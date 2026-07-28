package name.modid.effect;

import net.minecraft.core.Holder;
import net.minecraft.core.Registry;
import net.minecraft.core.registries.BuiltInRegistries;
import net.minecraft.core.registries.Registries;
import net.minecraft.resources.ResourceKey;
import net.minecraft.world.effect.MobEffect;
import net.minecraft.world.effect.MobEffectCategory;

import name.modid.EchoingVoid;
import name.modid.sound.ModSounds;

public class ModMobEffects {
	public static final int VOID_POISONED_DEFAULT_DURATION_TICKS = 260;

	private static final int VOID_POISONED_COLOR = 0x9C1FB0;

	public static final Holder<MobEffect> VOID_POISONED = register("void_poisoned",
		new VoidPoisonedMobEffect(MobEffectCategory.HARMFUL, VOID_POISONED_COLOR)
			.withSoundOnAdded(ModSounds.VOID_POISONED_ACTIVATE));

	private static Holder<MobEffect> register(String name, MobEffect effect) {
		ResourceKey<MobEffect> key = ResourceKey.create(Registries.MOB_EFFECT, EchoingVoid.id(name));
		return Registry.registerForHolder(BuiltInRegistries.MOB_EFFECT, key, effect);
	}

	public static void init() {
	}
}
