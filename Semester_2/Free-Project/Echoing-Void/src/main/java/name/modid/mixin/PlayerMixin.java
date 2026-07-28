package name.modid.mixin;

import org.spongepowered.asm.mixin.Mixin;
import org.spongepowered.asm.mixin.injection.At;
import org.spongepowered.asm.mixin.injection.Inject;
import org.spongepowered.asm.mixin.injection.callback.CallbackInfo;
import org.spongepowered.asm.mixin.injection.callback.CallbackInfoReturnable;

import net.minecraft.world.damagesource.DamageSource;
import net.minecraft.world.entity.Entity;
import net.minecraft.world.entity.item.ItemEntity;
import net.minecraft.world.entity.player.Player;
import net.minecraft.world.item.ItemStack;

import name.modid.item.ModItems;
import name.modid.item.TheBeginningOfTheEndCombo;
import name.modid.item.WeaponSounds;

@Mixin(Player.class)
public abstract class PlayerMixin {
	@Inject(method = "itemAttackInteraction", at = @At("HEAD"))
	private void echoingVoid$theBeginningOfTheEndCombo(Entity target, ItemStack weapon, DamageSource damageSource, boolean knockback, CallbackInfo ci) {
		if (weapon.is(ModItems.THE_BEGINNING_OF_THE_END)) {
			TheBeginningOfTheEndCombo.onHit((Player) (Object) this);
		}
	}

	@Inject(method = "drop", at = @At("HEAD"))
	private void echoingVoid$playDropSound(ItemStack stack, boolean includeThrowerName, CallbackInfoReturnable<ItemEntity> cir) {
		var sound = WeaponSounds.drop(stack);
		if (sound == null) {
			return;
		}
		Player self = (Player) (Object) this;
		self.level().playSound(null, self.getX(), self.getY(), self.getZ(), sound, self.getSoundSource(), 1.0F, 1.0F);
	}
}
