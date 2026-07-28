package name.modid.mixin;

import org.spongepowered.asm.mixin.Mixin;
import org.spongepowered.asm.mixin.injection.At;
import org.spongepowered.asm.mixin.injection.Inject;
import org.spongepowered.asm.mixin.injection.callback.CallbackInfoReturnable;

import net.minecraft.core.BlockPos;
import net.minecraft.sounds.SoundEvents;
import net.minecraft.sounds.SoundSource;
import net.minecraft.stats.Stats;
import net.minecraft.world.InteractionHand;
import net.minecraft.world.InteractionResult;
import net.minecraft.world.entity.player.Player;
import net.minecraft.world.item.BottleItem;
import net.minecraft.world.item.Item;
import net.minecraft.world.item.ItemStack;
import net.minecraft.world.item.ItemUtils;
import net.minecraft.world.level.ClipContext;
import net.minecraft.world.level.Level;
import net.minecraft.world.level.gameevent.GameEvent;
import net.minecraft.world.phys.BlockHitResult;
import net.minecraft.world.phys.HitResult;

import name.modid.fluid.ModFluids;
import name.modid.item.ModItems;

// Vanilla's glass bottle only knows how to fill itself from #minecraft:water (void_liquid is
// tagged into it purely for water-like swim/slowdown physics - see data/minecraft/tags/fluid/water.json
// and name.modid.fluid.VoidLiquidFluid), which would otherwise hand out a plain water bottle.
// Intercept before that check so scooping void liquid yields a Void Poisoned potion instead.
@Mixin(BottleItem.class)
public abstract class BottleItemMixin extends Item {
	public BottleItemMixin(Properties properties) {
		super(properties);
	}

	@Inject(method = "use", at = @At("HEAD"), cancellable = true)
	private void echoingVoid$fillWithVoidLiquid(Level level, Player player, InteractionHand hand, CallbackInfoReturnable<InteractionResult> cir) {
		ItemStack itemStack = player.getItemInHand(hand);
		BlockHitResult hitResult = getPlayerPOVHitResult(level, player, ClipContext.Fluid.SOURCE_ONLY);
		if (hitResult.getType() != HitResult.Type.BLOCK) {
			return;
		}
		BlockPos pos = hitResult.getBlockPos();
		if (!level.mayInteract(player, pos) || !level.getFluidState(pos).isSourceOfType(ModFluids.VOID_LIQUID)) {
			return;
		}

		level.playSound(player, player.getX(), player.getY(), player.getZ(), SoundEvents.BOTTLE_FILL, SoundSource.NEUTRAL, 1.0F, 1.0F);
		level.gameEvent(player, GameEvent.FLUID_PICKUP, pos);
		player.awardStat(Stats.ITEM_USED.get(this));
		ItemStack filled = ItemUtils.createFilledResult(itemStack, player, new ItemStack(ModItems.VOID_POISONED_POTION));
		cir.setReturnValue(InteractionResult.SUCCESS.heldItemTransformedTo(filled));
	}
}
