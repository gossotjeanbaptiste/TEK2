package name.modid.mixin;

import org.spongepowered.asm.mixin.Final;
import org.spongepowered.asm.mixin.Mixin;
import org.spongepowered.asm.mixin.Shadow;
import org.spongepowered.asm.mixin.injection.At;
import org.spongepowered.asm.mixin.injection.Inject;
import org.spongepowered.asm.mixin.injection.callback.CallbackInfo;

import net.minecraft.world.entity.player.Inventory;
import net.minecraft.world.entity.player.Player;
import net.minecraft.world.item.ItemStack;

import name.modid.item.WeaponSounds;

@Mixin(Inventory.class)
public abstract class InventoryMixin {
	@Shadow @Final public Player player;

	@Shadow public abstract ItemStack getSelectedItem();

	private ItemStack echoingVoid$previousSelected = ItemStack.EMPTY;

	@Inject(method = "setSelectedSlot", at = @At("HEAD"))
	private void echoingVoid$captureSelectedBeforeSwitch(int slot, CallbackInfo ci) {
		this.echoingVoid$previousSelected = this.getSelectedItem();
	}

	@Inject(method = "setSelectedSlot", at = @At("RETURN"))
	private void echoingVoid$playEquipSound(int slot, CallbackInfo ci) {
		ItemStack current = this.getSelectedItem();
		if (current.getItem() == this.echoingVoid$previousSelected.getItem()) {
			return;
		}
		var sound = WeaponSounds.equip(current);
		if (sound == null) {
			return;
		}
		this.player.level().playSound(null, this.player.getX(), this.player.getY(), this.player.getZ(),
			sound, this.player.getSoundSource(), 1.0F, 1.0F);
	}
}
