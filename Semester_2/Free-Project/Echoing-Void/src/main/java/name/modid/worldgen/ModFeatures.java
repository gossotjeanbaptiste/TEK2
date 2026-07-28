package name.modid.worldgen;

import net.minecraft.core.Registry;
import net.minecraft.core.registries.BuiltInRegistries;
import net.minecraft.core.registries.Registries;
import net.minecraft.resources.ResourceKey;
import net.minecraft.world.level.levelgen.feature.Feature;
import net.minecraft.world.level.levelgen.feature.configurations.NoneFeatureConfiguration;

import name.modid.EchoingVoid;

public class ModFeatures {
	public static final Feature<NoneFeatureConfiguration> VOID_ISLAND =
		register("void_island", new VoidIslandFeature(NoneFeatureConfiguration.CODEC));

	private static <F extends Feature<?>> F register(String name, F feature) {
		ResourceKey<Feature<?>> key = ResourceKey.create(Registries.FEATURE, EchoingVoid.id(name));
		return Registry.register(BuiltInRegistries.FEATURE, key, feature);
	}

	public static void init() {
	}
}
