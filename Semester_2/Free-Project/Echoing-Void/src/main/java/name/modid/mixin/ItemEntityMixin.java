package name.modid.mixin;

import org.spongepowered.asm.mixin.Mixin;
import org.spongepowered.asm.mixin.injection.At;
import org.spongepowered.asm.mixin.injection.Inject;
import org.spongepowered.asm.mixin.injection.callback.CallbackInfo;

import net.minecraft.world.entity.item.ItemEntity;
import net.minecraft.world.entity.player.Player;
import net.minecraft.world.item.ItemStack;

import name.modid.item.WeaponSounds;

@Mixin(ItemEntity.class)
public abstract class ItemEntityMixin {
	@Inject(method = "playerTouch", at = @At("HEAD"))
	private void echoingVoid$playPickUpSound(Player player, CallbackInfo ci) {
		ItemEntity self = (ItemEntity) (Object) this;
		ItemStack stack = self.getItem();
		var sound = WeaponSounds.pickUp(stack);
		if (sound == null) {
			return;
		}
		self.level().playSound(null, self.getX(), self.getY(), self.getZ(), sound, self.getSoundSource(), 1.0F, 1.0F);
	}
}
