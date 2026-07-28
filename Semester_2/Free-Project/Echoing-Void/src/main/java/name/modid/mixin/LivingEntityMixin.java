package name.modid.mixin;

import java.util.Collection;

import org.spongepowered.asm.mixin.Mixin;
import org.spongepowered.asm.mixin.injection.At;
import org.spongepowered.asm.mixin.injection.Inject;
import org.spongepowered.asm.mixin.injection.callback.CallbackInfo;
import org.spongepowered.asm.mixin.injection.callback.CallbackInfoReturnable;

import net.minecraft.world.effect.MobEffectInstance;
import net.minecraft.world.effect.MobEffects;
import net.minecraft.world.entity.EquipmentSlot;
import net.minecraft.world.entity.LivingEntity;

import name.modid.effect.ModMobEffects;
import name.modid.item.ModItems;
import name.modid.sound.ModSounds;
import name.modid.tag.ModEntityTypeTags;

@Mixin(LivingEntity.class)
public abstract class LivingEntityMixin {
	// The read (getEffect) happens every tick so losing the effect - from expiry or an external
	// clear, e.g. milk - is masked on the very same tick, never a visible gap. The actual write
	// (addEffect) only fires once the effect is gone/weaker, so a longer duration here just means
	// far fewer addEffect calls in the common case - not a longer window before noticing loss.
	private static final int ENDERITE_SET_SPEED_DURATION_TICKS = 60;
	private static final int ENDERITE_SET_SPEED_AMPLIFIER = 0;

	@Inject(method = "canBeAffected", at = @At("HEAD"), cancellable = true)
	private void echoingVoid$immuneToVoidPoisoned(MobEffectInstance newEffect, CallbackInfoReturnable<Boolean> cir) {
		LivingEntity self = (LivingEntity) (Object) this;
		if (newEffect.is(ModMobEffects.VOID_POISONED) && self.is(ModEntityTypeTags.IMMUNE_TO_VOID_POISONED)) {
			cir.setReturnValue(false);
		}
	}

	@Inject(method = "onEffectsRemoved", at = @At("HEAD"))
	private void echoingVoid$playVoidPoisonedDeactivateSound(Collection<MobEffectInstance> effects, CallbackInfo ci) {
		LivingEntity self = (LivingEntity) (Object) this;
		for (MobEffectInstance instance : effects) {
			if (instance.is(ModMobEffects.VOID_POISONED)) {
				self.level().playSound(null, self.getX(), self.getY(), self.getZ(),
					ModSounds.VOID_POISONED_DEACTIVATE, self.getSoundSource(), 1.0F, 1.0F);
				break;
			}
		}
	}

	@Inject(method = "tick", at = @At("TAIL"))
	private void echoingVoid$enderiteSetSpeedBonus(CallbackInfo ci) {
		LivingEntity self = (LivingEntity) (Object) this;
		if (self.level().isClientSide()) {
			return;
		}

		boolean fullEnderiteSet = self.getItemBySlot(EquipmentSlot.HEAD).is(ModItems.ENDERITE_HELMET)
			&& self.getItemBySlot(EquipmentSlot.CHEST).is(ModItems.ENDERITE_CHESTPLATE)
			&& self.getItemBySlot(EquipmentSlot.LEGS).is(ModItems.ENDERITE_LEGGINGS)
			&& self.getItemBySlot(EquipmentSlot.FEET).is(ModItems.ENDERITE_BOOTS);
		if (!fullEnderiteSet) {
			return;
		}

		MobEffectInstance current = self.getEffect(MobEffects.SPEED);
		if (current == null || current.getAmplifier() < ENDERITE_SET_SPEED_AMPLIFIER) {
			self.addEffect(new MobEffectInstance(MobEffects.SPEED, ENDERITE_SET_SPEED_DURATION_TICKS, ENDERITE_SET_SPEED_AMPLIFIER, false, false, true));
		}
	}
}
