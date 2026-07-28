package name.modid.effect;

import net.minecraft.server.level.ServerLevel;
import net.minecraft.world.effect.MobEffect;
import net.minecraft.world.effect.MobEffectCategory;
import net.minecraft.world.entity.LivingEntity;

public class VoidPoisonedMobEffect extends MobEffect {
	private static final int DAMAGE_INTERVAL_TICKS = 20;
	private static final float DAMAGE_PER_TICK = 3.0F;

	protected VoidPoisonedMobEffect(MobEffectCategory category, int color) {
		super(category, color);
	}

	@Override
	public boolean applyEffectTick(ServerLevel level, LivingEntity mob, int amplifier) {
		mob.hurtServer(level, mob.damageSources().magic(), DAMAGE_PER_TICK);
		return true;
	}

	@Override
	public boolean shouldApplyEffectTickThisTick(int tickCount, int amplifier) {
		int interval = DAMAGE_INTERVAL_TICKS >> amplifier;
		return interval <= 0 || tickCount % interval == 0;
	}
}
