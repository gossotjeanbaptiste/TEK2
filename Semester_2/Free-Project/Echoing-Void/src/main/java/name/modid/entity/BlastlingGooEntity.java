package name.modid.entity;

import net.minecraft.core.particles.ParticleTypes;
import net.minecraft.server.level.ServerLevel;
import net.minecraft.world.effect.MobEffectInstance;
import net.minecraft.world.entity.Entity;
import net.minecraft.world.entity.EntityType;
import net.minecraft.world.entity.LivingEntity;
import net.minecraft.world.entity.projectile.throwableitemprojectile.ThrowableItemProjectile;
import net.minecraft.world.item.Item;
import net.minecraft.world.item.ItemStack;
import net.minecraft.world.level.Level;
import net.minecraft.world.phys.EntityHitResult;
import net.minecraft.world.phys.HitResult;

import name.modid.effect.ModMobEffects;
import name.modid.item.ModItems;
import name.modid.sound.ModSounds;

/**
 * The rapid-fire purple goo bolt Blastlings shoot. Flies flat (no gravity arc) rather than
 * lobbing, unlike vanilla thrown projectiles, to read as fast "spit" rather than a thrown item.
 */
public class BlastlingGooEntity extends ThrowableItemProjectile {
	private static final float DAMAGE = 2.0F;
	// With no gravity this never arcs down to hit the ground on a miss - ThrowableProjectile.tick()
	// still applies its 0.99/tick inertia decay every tick regardless, so a shot that hits nothing
	// would otherwise just coast, decelerating forever, and never get discarded. Cap its lifetime
	// instead: at speed 1.6 (see BlastlingEntity#performRangedAttack) it crosses the 16-block ranged
	// attack radius in ~10 ticks, so 60 is generous headroom.
	private static final int MAX_LIFE_TICKS = 60;
	private static final float VOID_POISONED_CHANCE = 1.0F / 3.0F;
	private static final int VOID_POISONED_HIT_DURATION_TICKS = 60; // 3 seconds

	public BlastlingGooEntity(EntityType<? extends BlastlingGooEntity> type, Level level) {
		super(type, level);
	}

	public BlastlingGooEntity(Level level, LivingEntity owner, ItemStack itemStack) {
		super(ModEntities.BLASTLING_GOO, owner, level, itemStack);
	}

	@Override
	protected Item getDefaultItem() {
		return ModItems.BLASTLING_GOO;
	}

	@Override
	protected double getDefaultGravity() {
		return 0.0;
	}

	@Override
	public void tick() {
		super.tick();
		if (!this.level().isClientSide() && this.tickCount > MAX_LIFE_TICKS) {
			this.discard();
		}
	}

	@Override
	protected void onHitEntity(EntityHitResult hitResult) {
		super.onHitEntity(hitResult);
		Entity entity = hitResult.getEntity();
		if (this.level() instanceof ServerLevel serverLevel) {
			entity.hurtServer(serverLevel, this.damageSources().thrown(this, this.getOwner()), DAMAGE);
			if (entity instanceof LivingEntity target && this.random.nextFloat() < VOID_POISONED_CHANCE) {
				target.addEffect(new MobEffectInstance(ModMobEffects.VOID_POISONED, VOID_POISONED_HIT_DURATION_TICKS), this);
			}
		}
	}

	@Override
	public void handleEntityEvent(byte id) {
		if (id == 3) {
			for (int i = 0; i < 8; i++) {
				this.level().addParticle(ParticleTypes.PORTAL, this.getX(), this.getY(), this.getZ(),
					(this.random.nextDouble() - 0.5) * 2.0, -this.random.nextDouble(), (this.random.nextDouble() - 0.5) * 2.0);
			}
		}
	}

	@Override
	protected void onHit(HitResult hitResult) {
		super.onHit(hitResult);
		if (!this.level().isClientSide()) {
			this.level().broadcastEntityEvent(this, (byte) 3);
			this.playSound(ModSounds.BLASTLING_RANGED_ATTACK_IMPACT, 1.0F, 1.0F);
			this.discard();
		}
	}
}
