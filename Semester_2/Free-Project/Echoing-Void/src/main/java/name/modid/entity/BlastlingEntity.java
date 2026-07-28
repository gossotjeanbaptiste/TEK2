package name.modid.entity;

import net.minecraft.core.BlockPos;
import net.minecraft.core.Direction;
import net.minecraft.core.particles.ParticleTypes;
import net.minecraft.network.syncher.EntityDataAccessor;
import net.minecraft.network.syncher.EntityDataSerializers;
import net.minecraft.network.syncher.SynchedEntityData;
import net.minecraft.server.level.ServerLevel;
import net.minecraft.sounds.SoundEvent;
import net.minecraft.sounds.SoundEvents;
import net.minecraft.world.InteractionHand;
import net.minecraft.world.damagesource.DamageSource;
import net.minecraft.world.entity.EntityType;
import net.minecraft.world.entity.LivingEntity;
import net.minecraft.world.entity.ai.attributes.AttributeSupplier;
import net.minecraft.world.entity.ai.attributes.Attributes;
import net.minecraft.world.entity.ai.goal.FloatGoal;
import net.minecraft.world.entity.ai.goal.Goal;
import net.minecraft.world.entity.ai.goal.LookAtPlayerGoal;
import net.minecraft.world.entity.ai.goal.RandomLookAroundGoal;
import net.minecraft.world.entity.ai.goal.RangedAttackGoal;
import net.minecraft.world.entity.ai.goal.WaterAvoidingRandomStrollGoal;
import net.minecraft.world.entity.ai.goal.target.HurtByTargetGoal;
import net.minecraft.world.entity.ai.goal.target.NearestAttackableTargetGoal;
import net.minecraft.world.entity.monster.Monster;
import net.minecraft.world.entity.monster.RangedAttackMob;
import net.minecraft.world.entity.player.Player;
import net.minecraft.world.item.ItemStack;
import net.minecraft.world.level.Level;
import net.minecraft.world.phys.Vec3;

import name.modid.item.ModItems;
import name.modid.sound.ModSounds;

/**
 * Only two behaviors, per the Dungeons original: a rapid ranged goo attack, and teleporting away
 * both when a hero closes in and whenever it takes damage - no melee.
 */
public class BlastlingEntity extends Monster implements RangedAttackMob {
	private static final float TOO_CLOSE_RADIUS = 6.0F;
	private static final float TOO_CLOSE_RADIUS_SQR = TOO_CLOSE_RADIUS * TOO_CLOSE_RADIUS;
	private static final float RANGED_ATTACK_RADIUS = 16.0F;
	private static final int RANGED_ATTACK_INTERVAL_MIN = 8;
	private static final int RANGED_ATTACK_INTERVAL_MAX = 16;
	private static final int TELEPORT_HIDE_MIN_TICKS = 1;
	private static final int TELEPORT_HIDE_RANDOM_TICKS = 1;

	private static final EntityDataAccessor<Boolean> DATA_TELEPORT_HIDDEN =
		SynchedEntityData.defineId(BlastlingEntity.class, EntityDataSerializers.BOOLEAN);

	private int teleportHideTicks;

	public BlastlingEntity(EntityType<? extends BlastlingEntity> type, Level level) {
		super(type, level);
	}

	public boolean isTeleportHidden() {
		return this.entityData.get(DATA_TELEPORT_HIDDEN);
	}

	@Override
	protected void defineSynchedData(SynchedEntityData.Builder builder) {
		super.defineSynchedData(builder);
		builder.define(DATA_TELEPORT_HIDDEN, false);
	}

	public static AttributeSupplier.Builder createAttributes() {
		return Monster.createMonsterAttributes()
			.add(Attributes.MAX_HEALTH, 16.0)
			.add(Attributes.MOVEMENT_SPEED, 0.3)
			.add(Attributes.FOLLOW_RANGE, 24.0)
			.add(Attributes.ARMOR, 0.0);
	}

	@Override
	protected void registerGoals() {
		this.goalSelector.addGoal(0, new FloatGoal(this));
		this.goalSelector.addGoal(1, new TeleportAwayGoal(this));
		this.goalSelector.addGoal(2, new RangedAttackGoal(this, 1.0, RANGED_ATTACK_INTERVAL_MIN, RANGED_ATTACK_INTERVAL_MAX, RANGED_ATTACK_RADIUS));
		this.goalSelector.addGoal(6, new WaterAvoidingRandomStrollGoal(this, 1.0));
		this.goalSelector.addGoal(7, new LookAtPlayerGoal(this, Player.class, 8.0F));
		this.goalSelector.addGoal(7, new RandomLookAroundGoal(this));

		this.targetSelector.addGoal(1, new HurtByTargetGoal(this).setAlertOthers());
		this.targetSelector.addGoal(2, new NearestAttackableTargetGoal<>(this, Player.class, 5, true, false, null));
	}

	@Override
	protected SoundEvent getAmbientSound() {
		return ModSounds.BLASTLING_IDLE;
	}

	@Override
	protected SoundEvent getHurtSound(DamageSource source) {
		return ModSounds.BLASTLING_HURT;
	}

	@Override
	protected SoundEvent getDeathSound() {
		return ModSounds.BLASTLING_DEATH;
	}

	@Override
	public boolean hurtServer(ServerLevel level, DamageSource source, float amount) {
		boolean hurt = super.hurtServer(level, source, amount);
		if (hurt && this.isAlive() && source.getEntity() != this) {
			this.teleportEscape();
		}
		return hurt;
	}

	@Override
	protected void customServerAiStep(ServerLevel level) {
		super.customServerAiStep(level);
		if (this.teleportHideTicks > 0) {
			this.teleportHideTicks--;
			if (this.teleportHideTicks == 0) {
				this.entityData.set(DATA_TELEPORT_HIDDEN, false);
				this.playSound(SoundEvents.ENDERMAN_TELEPORT, 1.0F, 1.0F);
			}
		}
	}

	@Override
	public void performRangedAttack(LivingEntity target, float power) {
		if (!(this.level() instanceof ServerLevel)) {
			return;
		}
		// The goo has no gravity (see BlastlingGooEntity#getDefaultGravity), so this is a straight
		// shot: aim directly from the actual spawn point (matches ThrowableItemProjectile's ctor,
		// which spawns at getEyeY() - 0.1) at the target's eye height - no arc/lob compensation.
		Vec3 targetMovement = target.getDeltaMovement();
		double spawnEyeY = this.getEyeY() - 0.1;
		double xd = target.getX() + targetMovement.x - this.getX();
		double yd = target.getEyeY() + targetMovement.y - spawnEyeY;
		double zd = target.getZ() + targetMovement.z - this.getZ();

		BlastlingGooEntity goo = new BlastlingGooEntity(this.level(), this, new ItemStack(ModItems.BLASTLING_GOO));
		goo.shoot(xd, yd, zd, 1.6F, 2.0F);
		this.level().addFreshEntity(goo);

		this.swing(InteractionHand.MAIN_HAND);
		this.playSound(ModSounds.BLASTLING_RANGED_ATTACK_ANIMATION, 1.0F, 1.0F);
		this.playSound(ModSounds.BLASTLING_RANGE_ATTACK_PROJ, 1.0F, 1.0F);
	}

	private boolean teleportAwayFrom(LivingEntity target) {
		double dx = this.getX() - target.getX();
		double dz = this.getZ() - target.getZ();
		double horizontalDist = Math.sqrt(dx * dx + dz * dz);
		if (horizontalDist < 1.0E-2) {
			dx = this.random.nextDouble() - 0.5;
			dz = this.random.nextDouble() - 0.5;
			horizontalDist = Math.sqrt(dx * dx + dz * dz);
		}
		double nx = dx / horizontalDist;
		double nz = dz / horizontalDist;
		double distance = 10.0 + this.random.nextDouble() * 6.0;
		double x = this.getX() + nx * distance + (this.random.nextDouble() - 0.5) * 4.0;
		double y = this.getY();
		double z = this.getZ() + nz * distance + (this.random.nextDouble() - 0.5) * 4.0;
		return teleport(x, y, z);
	}

	private void teleportEscape() {
		double x = this.getX() + (this.random.nextDouble() - 0.5) * 16.0;
		double y = this.getY() + (this.random.nextInt(16) - 8);
		double z = this.getZ() + (this.random.nextDouble() - 0.5) * 16.0;
		teleport(x, y, z);
	}

	private boolean teleport(double x, double y, double z) {
		if (!(this.level() instanceof ServerLevel serverLevel)) {
			return false;
		}

		// Scan down from the requested spot to the nearest solid ground, like vanilla EnderMan does,
		// otherwise randomTeleport frequently rejects the raw offset (e.g. mid-air or underground) and silently fails.
		BlockPos.MutableBlockPos pos = new BlockPos.MutableBlockPos(x, y, z);
		while (pos.getY() > serverLevel.getMinY() && !serverLevel.getBlockState(pos).isFaceSturdy(serverLevel, pos, Direction.UP)) {
			pos.move(Direction.DOWN);
		}
		if (!serverLevel.getBlockState(pos).isFaceSturdy(serverLevel, pos, Direction.UP)) {
			return false;
		}
		x = pos.getX() + 0.5;
		y = pos.getY() + 1.0;
		z = pos.getZ() + 0.5;

		Vec3 oldPos = this.position();
		boolean success = this.randomTeleport(x, y, z, true);
		if (success) {
			double eyeOffset = this.getBbHeight() * 0.5;
			serverLevel.sendParticles(ParticleTypes.PORTAL, oldPos.x, oldPos.y + eyeOffset, oldPos.z, 32, 0.5, 0.5, 0.5, 0.0);
			serverLevel.sendParticles(ParticleTypes.PORTAL, this.getX(), this.getY() + eyeOffset, this.getZ(), 32, 0.5, 0.5, 0.5, 0.0);
			this.playSound(SoundEvents.ENDERMAN_TELEPORT, 1.0F, 1.0F);
			int hideDuration = TELEPORT_HIDE_MIN_TICKS + this.random.nextInt(TELEPORT_HIDE_RANDOM_TICKS);
			this.entityData.set(DATA_TELEPORT_HIDDEN, true);
			this.teleportHideTicks = hideDuration;
		}
		return success;
	}

	/**
	 * Fires whenever a target closes inside {@link #TOO_CLOSE_RADIUS} - the mirror image of
	 * {@code WatchlingEntity.TeleportToTargetGoal}, which instead teleports the Watchling *towards*
	 * a target that's too far away.
	 */
	private static class TeleportAwayGoal extends Goal {
		private final BlastlingEntity blastling;
		private int cooldown;

		TeleportAwayGoal(BlastlingEntity blastling) {
			this.blastling = blastling;
		}

		@Override
		public boolean canUse() {
			LivingEntity target = this.blastling.getTarget();
			if (target == null || !target.isAlive()) {
				return false;
			}
			if (this.cooldown > 0) {
				this.cooldown--;
				return false;
			}
			return this.blastling.distanceToSqr(target) < TOO_CLOSE_RADIUS_SQR;
		}

		@Override
		public boolean canContinueToUse() {
			return false;
		}

		@Override
		public void start() {
			LivingEntity target = this.blastling.getTarget();
			if (target != null) {
				this.blastling.teleportAwayFrom(target);
			}
			this.cooldown = 10 + this.blastling.random.nextInt(10);
		}
	}
}
