package name.modid.fluid;

import java.util.Optional;
import java.util.Map;
import java.util.WeakHashMap;

import net.minecraft.core.BlockPos;
import net.minecraft.core.Direction;
import net.minecraft.core.particles.ParticleOptions;
import net.minecraft.core.particles.ParticleTypes;
import net.minecraft.core.registries.BuiltInRegistries;
import net.minecraft.server.level.ServerLevel;
import net.minecraft.sounds.SoundEvent;
import net.minecraft.sounds.SoundEvents;
import net.minecraft.util.RandomSource;
import net.minecraft.world.effect.MobEffectInstance;
import net.minecraft.world.entity.Entity;
import net.minecraft.world.entity.InsideBlockEffectApplier;
import net.minecraft.world.entity.InsideBlockEffectType;
import net.minecraft.world.entity.LivingEntity;
import net.minecraft.world.item.Item;
import net.minecraft.world.level.BlockGetter;
import net.minecraft.world.level.Level;
import net.minecraft.world.level.LevelAccessor;
import net.minecraft.world.level.LevelReader;
import net.minecraft.world.level.block.Block;
import net.minecraft.world.level.block.LiquidBlock;
import net.minecraft.world.level.block.entity.BlockEntity;
import net.minecraft.world.level.block.state.BlockState;
import net.minecraft.world.level.block.state.StateDefinition;
import net.minecraft.tags.FluidTags;
import net.minecraft.world.level.material.Fluid;
import net.minecraft.world.level.material.FlowingFluid;
import net.minecraft.world.level.material.FluidState;

import name.modid.blocks.ModBlocks;
import name.modid.effect.ModMobEffects;
import name.modid.item.ModItems;

// Dark purple, water-like liquid from the Echoing Void. void_liquid/flowing_void_liquid are added
// to #minecraft:water (see data/minecraft/tags/fluid/water.json) purely so entities get water's
// swim/slowdown physics and submersion handling for free - BottleItemMixin intercepts glass bottle
// filling before vanilla's hardcoded water-tag check so it yields a Void Poisoned potion instead
// of a water bottle.
public abstract class VoidLiquidFluid extends FlowingFluid {
	private static final int VOID_POISON_EXPOSURE_TICKS = 60; // 3 seconds of continuous submersion
	private static final Map<LivingEntity, Integer> SUBMERSION_TICKS = new WeakHashMap<>();

	@Override
	public Fluid getFlowing() {
		return ModFluids.FLOWING_VOID_LIQUID;
	}

	@Override
	public Fluid getSource() {
		return ModFluids.VOID_LIQUID;
	}

	@Override
	public Item getBucket() {
		return ModItems.VOID_LIQUID_BUCKET;
	}

	@Override
	public void animateTick(Level level, BlockPos pos, FluidState fluidState, RandomSource random) {
		if (random.nextInt(20) == 0) {
			level.addParticle(ParticleTypes.PORTAL,
				pos.getX() + random.nextDouble(),
				pos.getY() + random.nextDouble(),
				pos.getZ() + random.nextDouble(),
				(random.nextDouble() - 0.5) * 0.5, -random.nextDouble(), (random.nextDouble() - 0.5) * 0.5);
		}
	}

	@Override
	public ParticleOptions getDripParticle() {
		return ParticleTypes.DRIPPING_OBSIDIAN_TEAR;
	}

	@Override
	protected boolean canConvertToSource(ServerLevel level) {
		return false;
	}

	@Override
	protected void beforeDestroyingBlock(LevelAccessor level, BlockPos pos, BlockState state) {
		BlockEntity blockEntity = state.hasBlockEntity() ? level.getBlockEntity(pos) : null;
		Block.dropResources(state, level, pos, blockEntity);
	}

	@Override
	protected void entityInside(Level level, BlockPos pos, Entity entity, InsideBlockEffectApplier effectApplier) {
		effectApplier.apply(InsideBlockEffectType.EXTINGUISH);
		effectApplier.runAfter(InsideBlockEffectType.EXTINGUISH, VoidLiquidFluid::applyVoidExposure);
	}

	private static void applyVoidExposure(Entity entity) {
		if (!(entity instanceof LivingEntity living) || !(living.level() instanceof ServerLevel)) {
			return;
		}
		int ticks = SUBMERSION_TICKS.merge(living, 1, Integer::sum);
		if (ticks >= VOID_POISON_EXPOSURE_TICKS) {
			SUBMERSION_TICKS.remove(living);
			living.addEffect(new MobEffectInstance(ModMobEffects.VOID_POISONED, ModMobEffects.VOID_POISONED_DEFAULT_DURATION_TICKS));
		}
	}

	@Override
	public int getSlopeFindDistance(LevelReader level) {
		return 4;
	}

	@Override
	public BlockState createLegacyBlock(FluidState fluidState) {
		return ModBlocks.VOID_LIQUID.defaultBlockState().setValue(LiquidBlock.LEVEL, getLegacyLevel(fluidState));
	}

	@Override
	public boolean isSame(Fluid other) {
		return other == ModFluids.VOID_LIQUID || other == ModFluids.FLOWING_VOID_LIQUID;
	}

	@Override
	public int getDropOff(LevelReader level) {
		return 1;
	}

	@Override
	public int getTickDelay(LevelReader level) {
		return 5;
	}

	@Override
	public boolean canBeReplacedWith(FluidState state, BlockGetter level, BlockPos pos, Fluid other, Direction direction) {
		return direction == Direction.DOWN && !BuiltInRegistries.FLUID.wrapAsHolder(other).is(FluidTags.WATER);
	}

	@Override
	protected float getExplosionResistance() {
		return 100.0F;
	}

	@Override
	public Optional<SoundEvent> getPickupSound() {
		return Optional.of(SoundEvents.BUCKET_FILL);
	}

	public static class Flowing extends VoidLiquidFluid {
		@Override
		protected void createFluidStateDefinition(StateDefinition.Builder<Fluid, FluidState> builder) {
			super.createFluidStateDefinition(builder);
			builder.add(LEVEL);
		}

		@Override
		public int getAmount(FluidState fluidState) {
			return fluidState.getValue(LEVEL);
		}

		@Override
		public boolean isSource(FluidState fluidState) {
			return false;
		}
	}

	public static class Source extends VoidLiquidFluid {
		@Override
		public int getAmount(FluidState fluidState) {
			return 8;
		}

		@Override
		public boolean isSource(FluidState fluidState) {
			return true;
		}
	}
}
