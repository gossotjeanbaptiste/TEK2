package name.modid.entity;

import java.util.EnumSet;

import net.minecraft.core.BlockPos;
import net.minecraft.core.Direction;
import net.minecraft.core.particles.ParticleTypes;
import net.minecraft.network.syncher.EntityDataAccessor;
import net.minecraft.network.syncher.EntityDataSerializers;
import net.minecraft.network.syncher.SynchedEntityData;
import net.minecraft.server.level.ServerLevel;
import net.minecraft.sounds.SoundEvent;
import net.minecraft.world.InteractionHand;
import net.minecraft.world.damagesource.DamageSource;
import net.minecraft.world.effect.MobEffectInstance;
import net.minecraft.world.entity.EntityType;
import net.minecraft.world.entity.LivingEntity;
import net.minecraft.world.entity.ai.attributes.AttributeSupplier;
import net.minecraft.world.entity.ai.attributes.Attributes;
import net.minecraft.world.entity.ai.goal.FloatGoal;
import net.minecraft.world.entity.ai.goal.Goal;
import net.minecraft.world.entity.ai.goal.LookAtPlayerGoal;
import net.minecraft.world.entity.ai.goal.RandomLookAroundGoal;
import net.minecraft.world.entity.ai.goal.WaterAvoidingRandomStrollGoal;
import net.minecraft.world.entity.ai.goal.target.HurtByTargetGoal;
import net.minecraft.world.entity.ai.goal.target.NearestAttackableTargetGoal;
import net.minecraft.world.entity.monster.Monster;
import net.minecraft.world.entity.player.Player;
import net.minecraft.world.level.Level;
import net.minecraft.world.phys.Vec3;

import name.modid.effect.ModMobEffects;
import name.modid.sound.ModSounds;

public class WatchlingEntity extends Monster {
	private static final float HEAVY_ATTACK_MULTIPLIER = 1.65F;
	private static final float MELEE_REACH_SQR = 6.25F;
	private static final float TELEPORT_ESCAPE_CHANCE = 0.5F;
	private static final float HEAVY_ATTACK_CHANCE = 0.25F;
	private static final int TELEPORT_HIDE_MIN_TICKS = 1;
	private static final int TELEPORT_HIDE_RANDOM_TICKS = 1;
	private static final int WATER_DAMAGE_INTERVAL_TICKS = 20;
	private static final float WATER_DAMAGE_AMOUNT = 2.0F;

	private static final EntityDataAccessor<Boolean> DATA_TELEPORT_HIDDEN =
		SynchedEntityData.defineId(WatchlingEntity.class, EntityDataSerializers.BOOLEAN);

	private int teleportHideTicks;
	private int waterDamageCooldown;
	private boolean tookWaterDamage;

	public WatchlingEntity(EntityType<? extends WatchlingEntity> type, Level level) {
		super(type, level);
	}

	@Override
	public boolean isSensitiveToWater() {
		return true;
	}

	@Override
	public void aiStep() {
		// Same constant ambient portal-particle trail as vanilla EnderMan, whether idle or moving.
		if (this.level().isClientSide()) {
			for (int i = 0; i < 2; i++) {
				this.level()
					.addParticle(
						ParticleTypes.PORTAL,
						this.getRandomX(0.5),
						this.getRandomY() - 0.25,
						this.getRandomZ(0.5),
						(this.random.nextDouble() - 0.5) * 2.0,
						-this.random.nextDouble(),
						(this.random.nextDouble() - 0.5) * 2.0
					);
			}
		}
		super.aiStep();
	}

	@Override
	protected void defineSynchedData(SynchedEntityData.Builder builder) {
		super.defineSynchedData(builder);
		builder.define(DATA_TELEPORT_HIDDEN, false);
	}

	public static AttributeSupplier.Builder createAttributes() {
		return Monster.createMonsterAttributes()
			.add(Attributes.MAX_HEALTH, 30.0)
			.add(Attributes.MOVEMENT_SPEED, 0.3)
			.add(Attributes.ATTACK_DAMAGE, 4.0)
			.add(Attributes.FOLLOW_RANGE, 16.0)
			.add(Attributes.ARMOR, 0.0);
	}

	@Override
	protected void registerGoals() {
		this.goalSelector.addGoal(0, new FloatGoal(this));
		this.goalSelector.addGoal(1, new TeleportToTargetGoal(this));
		this.goalSelector.addGoal(2, new AttackGoal(this));
		this.goalSelector.addGoal(3, new StareGoal(this));
		this.goalSelector.addGoal(7, new WaterAvoidingRandomStrollGoal(this, 1.0));
		this.goalSelector.addGoal(8, new LookAtPlayerGoal(this, Player.class, 8.0F));
		this.goalSelector.addGoal(8, new RandomLookAroundGoal(this));

		this.targetSelector.addGoal(1, new HurtByTargetGoal(this).setAlertOthers());
		this.targetSelector.addGoal(2, new NearestAttackableTargetGoal<>(this, Player.class, 5, true, false, null));
	}

	@Override
	protected SoundEvent getAmbientSound() {
		return ModSounds.WATCHLING_IDLE;
	}

	@Override
	public int getAmbientSoundInterval() {
		// Vanilla default (80) gives an average ~6s gap; this chatters more often, fitting a
		// creature covered in restless eyes.
		return 25;
	}

	@Override
	protected SoundEvent getHurtSound(DamageSource source) {
		return ModSounds.WATCHLING_HURT;
	}

	@Override
	protected SoundEvent getDeathSound() {
		return ModSounds.WATCHLING_DEATH;
	}

	@Override
	public boolean hurtServer(ServerLevel level, DamageSource source, float amount) {
		boolean hurt = super.hurtServer(level, source, amount);
		if (hurt && this.isAlive() && source.getEntity() != this && this.random.nextFloat() < TELEPORT_ESCAPE_CHANCE) {
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
				this.playSound(ModSounds.WATCHLING_TELEPORT_END, 1.0F, 1.0F);
			}
		}

		if (this.isInWater()) {
			if (!this.tookWaterDamage) {
				this.hurtServer(level, this.damageSources().generic(), WATER_DAMAGE_AMOUNT);
				this.waterDamageCooldown = WATER_DAMAGE_INTERVAL_TICKS;
				this.tookWaterDamage = true;
				this.teleportEscape();
			} else if (this.waterDamageCooldown <= 0) {
				this.hurtServer(level, this.damageSources().generic(), WATER_DAMAGE_AMOUNT);
				this.waterDamageCooldown = WATER_DAMAGE_INTERVAL_TICKS;
			} else {
				this.waterDamageCooldown--;
			}
		} else {
			this.waterDamageCooldown = 0;
			this.tookWaterDamage = false;
		}
	}

	private void performAttack(LivingEntity target, boolean heavy) {
		if (!(this.level() instanceof ServerLevel serverLevel)) {
			return;
		}
		this.swing(InteractionHand.MAIN_HAND);
		this.playSound(heavy ? ModSounds.WATCHLING_HEAVY_ATTACK : ModSounds.WATCHLING_ATTACK, 1.0F, 1.0F);
		float base = (float) this.getAttributeValue(Attributes.ATTACK_DAMAGE);
		float damage = heavy ? base * HEAVY_ATTACK_MULTIPLIER : base;
		target.hurtServer(serverLevel, this.level().damageSources().mobAttack(this), damage);
		if (heavy) {
			target.addEffect(new MobEffectInstance(ModMobEffects.VOID_POISONED, ModMobEffects.VOID_POISONED_DEFAULT_DURATION_TICKS), this);
		}
	}

	private boolean teleportTowards(LivingEntity target) {
		double x = target.getX() + (this.random.nextDouble() - 0.5) * 4.0;
		double y = target.getY();
		double z = target.getZ() + (this.random.nextDouble() - 0.5) * 4.0;
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
			this.playSound(ModSounds.WATCHLING_TELEPORT_START, 1.0F, 1.0F);
			int hideDuration = TELEPORT_HIDE_MIN_TICKS + this.random.nextInt(TELEPORT_HIDE_RANDOM_TICKS);
			this.entityData.set(DATA_TELEPORT_HIDDEN, true);
			this.teleportHideTicks = hideDuration;
		}
		return success;
	}

	private static class TeleportToTargetGoal extends Goal {
		private final WatchlingEntity watchling;
		private int cooldown;

		TeleportToTargetGoal(WatchlingEntity watchling) {
			this.watchling = watchling;
		}

		@Override
		public boolean canUse() {
			LivingEntity target = this.watchling.getTarget();
			if (target == null || !target.isAlive()) {
				return false;
			}
			if (this.cooldown > 0) {
				this.cooldown--;
				return false;
			}
			return this.watchling.distanceToSqr(target) > MELEE_REACH_SQR || !this.watchling.hasLineOfSight(target);
		}

		@Override
		public boolean canContinueToUse() {
			return false;
		}

		@Override
		public void start() {
			LivingEntity target = this.watchling.getTarget();
			if (target != null)
				this.watchling.teleportTowards(target);
			this.cooldown = 10 + this.watchling.random.nextInt(10);
		}
	}

	private static class AttackGoal extends Goal {
		private final WatchlingEntity watchling;
		private int attackCooldown;

		AttackGoal(WatchlingEntity watchling) {
			this.watchling = watchling;
			this.setFlags(EnumSet.of(Goal.Flag.LOOK));
		}

		@Override
		public boolean canUse() {
			LivingEntity target = this.watchling.getTarget();
			return target != null && target.isAlive()
				&& this.watchling.distanceToSqr(target) <= MELEE_REACH_SQR
				&& this.watchling.hasLineOfSight(target);
		}

		@Override
		public boolean canContinueToUse() {
			return canUse();
		}

		@Override
		public void start() {
			this.attackCooldown = 0;
		}

		@Override
		public void tick() {
			LivingEntity target = this.watchling.getTarget();
			if (target == null) {
				return;
			}
			this.watchling.getLookControl().setLookAt(target, 30.0F, 30.0F);
			if (this.attackCooldown-- <= 0) {
				boolean heavy = this.watchling.random.nextFloat() < HEAVY_ATTACK_CHANCE;
				this.watchling.performAttack(target, heavy);
				this.attackCooldown = heavy ? 40 : 20;
			}
		}
	}

	/**
	 * Between 16 and 32 blocks (i.e. beyond the follow range so it never becomes an actual combat
	 * target), a Watchling that can directly see a player freezes in place and stares them down.
	 */
	private static class StareGoal extends Goal {
		private static final double MIN_RANGE = 16.0;
		private static final double MAX_RANGE = 32.0;

		private final WatchlingEntity watchling;
		private Player watchedPlayer;

		StareGoal(WatchlingEntity watchling) {
			this.watchling = watchling;
			this.setFlags(EnumSet.of(Goal.Flag.MOVE, Goal.Flag.LOOK));
		}

		@Override
		public boolean canUse() {
			if (this.watchling.getTarget() != null) {
				return false;
			}
			Player player = this.watchling.level().getNearestPlayer(this.watchling, MAX_RANGE);
			if (player == null || !player.isAlive() || player.isSpectator()) {
				return false;
			}
			double distanceSqr = this.watchling.distanceToSqr(player);
			if (distanceSqr < MIN_RANGE * MIN_RANGE || distanceSqr > MAX_RANGE * MAX_RANGE) {
				return false;
			}
			if (!this.watchling.hasLineOfSight(player)) {
				return false;
			}
			this.watchedPlayer = player;
			return true;
		}

		@Override
		public boolean canContinueToUse() {
			if (this.watchling.getTarget() != null || this.watchedPlayer == null || !this.watchedPlayer.isAlive()) {
				return false;
			}
			double distanceSqr = this.watchling.distanceToSqr(this.watchedPlayer);
			return distanceSqr >= MIN_RANGE * MIN_RANGE && distanceSqr <= MAX_RANGE * MAX_RANGE
				&& this.watchling.hasLineOfSight(this.watchedPlayer);
		}

		@Override
		public void start() {
			this.watchling.getNavigation().stop();
		}

		@Override
		public void stop() {
			this.watchedPlayer = null;
		}

		@Override
		public void tick() {
			this.watchling.getLookControl().setLookAt(this.watchedPlayer, 30.0F, 30.0F);
		}
	}
}
