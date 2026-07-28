package name.modid.mixin;

import org.spongepowered.asm.mixin.Mixin;
import org.spongepowered.asm.mixin.injection.At;
import org.spongepowered.asm.mixin.injection.Inject;
import org.spongepowered.asm.mixin.injection.callback.CallbackInfoReturnable;

import net.minecraft.world.Container;
import net.minecraft.world.inventory.Slot;
import net.minecraft.world.item.ItemStack;
import net.minecraft.world.item.Items;

import name.modid.block.entity.EndBrewingStandBlockEntity;

@Mixin(targets = "net.minecraft.world.inventory.BrewingStandMenu$IngredientsSlot")
public class BrewingStandIngredientsSlotMixin extends Slot {
	public BrewingStandIngredientsSlotMixin(Container container, int slot, int x, int y) {
		super(container, slot, x, y);
	}

	@Inject(method = "mayPlace", at = @At("HEAD"), cancellable = true)
	private void echoingVoid$allowEnderPearlForEndBrewingStand(ItemStack itemStack, CallbackInfoReturnable<Boolean> cir) {
		if (this.container instanceof EndBrewingStandBlockEntity && itemStack.is(Items.ENDER_PEARL)) {
			cir.setReturnValue(true);
		}
	}
}
