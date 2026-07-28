package name.modid.mixin;

import org.spongepowered.asm.mixin.Mixin;
import org.spongepowered.asm.mixin.injection.At;
import org.spongepowered.asm.mixin.injection.Inject;
import org.spongepowered.asm.mixin.injection.callback.CallbackInfo;

import net.minecraft.world.entity.projectile.arrow.AbstractArrow;
import net.minecraft.world.item.ItemStack;
import net.minecraft.world.phys.BlockHitResult;
import net.minecraft.world.phys.EntityHitResult;

import name.modid.item.CallOfTheVoidBowItem;
import name.modid.item.ModItems;
import name.modid.sound.ModSounds;

@Mixin(AbstractArrow.class)
public abstract class AbstractArrowMixin {
	@Inject(method = "onHitEntity", at = @At("HEAD"))
	private void echoingVoid$callOfTheVoidImpactOnEntity(EntityHitResult hitResult, CallbackInfo ci) {
		AbstractArrow self = (AbstractArrow) (Object) this;
		echoingVoid$playImpactSound(self);
	}

	@Inject(method = "onHitBlock", at = @At("HEAD"))
	private void echoingVoid$callOfTheVoidImpactOnBlock(BlockHitResult hitResult, CallbackInfo ci) {
		AbstractArrow self = (AbstractArrow) (Object) this;
		echoingVoid$playImpactSound(self);
	}

	private static void echoingVoid$playImpactSound(AbstractArrow arrow) {
		ItemStack weapon = arrow.getWeaponItem();
		if (weapon == null || !weapon.is(ModItems.CALL_OF_THE_VOID)) {
			return;
		}
		boolean chargedShot = arrow.entityTags().contains(CallOfTheVoidBowItem.CHARGED_SHOT_TAG);
		arrow.level().playSound(null, arrow.getX(), arrow.getY(), arrow.getZ(),
			chargedShot ? ModSounds.CALL_OF_THE_VOID_CHARGE_IMPACT : ModSounds.CALL_OF_THE_VOID_IMPACT,
			arrow.getSoundSource(), 1.0F, 1.0F);
	}
}
