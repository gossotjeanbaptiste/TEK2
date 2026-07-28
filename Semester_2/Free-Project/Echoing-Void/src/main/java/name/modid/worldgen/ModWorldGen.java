package name.modid.worldgen;

import net.fabricmc.fabric.api.biome.v1.BiomeModifications;
import net.fabricmc.fabric.api.biome.v1.BiomeSelectors;
import net.fabricmc.fabric.api.biome.v1.TheEndBiomes;

import net.minecraft.core.registries.Registries;
import net.minecraft.resources.ResourceKey;
import net.minecraft.world.level.biome.Biome;
import net.minecraft.world.level.biome.Biomes;
import net.minecraft.world.level.levelgen.GenerationStep;
import net.minecraft.world.level.levelgen.placement.PlacedFeature;

import name.modid.EchoingVoid;

public class ModWorldGen {
	public static final ResourceKey<Biome> VOID_ISLANDS =
		ResourceKey.create(Registries.BIOME, EchoingVoid.id("void_islands"));

	// Weighted against vanilla Small End Islands' own baseline weight of 1.0 (see
	// Fabric API's TheEndBiomeData) - dominant but not absolute, so the vanilla biome can still
	// rarely appear (keeps the "Adventuring Time" advancement, which requires visiting
	// minecraft:small_end_islands specifically, completable).
	private static final double VOID_ISLANDS_WEIGHT = 20.0;

	private static final ResourceKey<PlacedFeature> ENDERITE_DEBRIS_LARGE =
		ResourceKey.create(Registries.PLACED_FEATURE, EchoingVoid.id("enderite_debris_large"));
	private static final ResourceKey<PlacedFeature> ENDERITE_DEBRIS_SMALL =
		ResourceKey.create(Registries.PLACED_FEATURE, EchoingVoid.id("enderite_debris_small"));
	private static final ResourceKey<PlacedFeature> VOID_LIQUID_POOL =
		ResourceKey.create(Registries.PLACED_FEATURE, EchoingVoid.id("void_liquid_pool"));

	public static void init() {
		TheEndBiomes.addSmallIslandsBiome(VOID_ISLANDS, VOID_ISLANDS_WEIGHT);

		// Same pairing (large vein + small vein, each placed once per chunk, same
		// discard-on-air-exposure/height-range shape) that vanilla uses for ancient debris in the
		// nether - only in the outer End islands, not the central main island.
		BiomeModifications.addFeature(
			BiomeSelectors.includeByKey(Biomes.END_HIGHLANDS, Biomes.END_MIDLANDS, Biomes.END_BARRENS, Biomes.SMALL_END_ISLANDS, VOID_ISLANDS),
			GenerationStep.Decoration.UNDERGROUND_DECORATION,
			ENDERITE_DEBRIS_LARGE);
		BiomeModifications.addFeature(
			BiomeSelectors.includeByKey(Biomes.END_HIGHLANDS, Biomes.END_MIDLANDS, Biomes.END_BARRENS, Biomes.SMALL_END_ISLANDS, VOID_ISLANDS),
			GenerationStep.Decoration.UNDERGROUND_DECORATION,
			ENDERITE_DEBRIS_SMALL);

		// Every outer End biome (not the central main island) - small surface pools of Void Liquid,
		// same "lake" feature shape vanilla uses for its rare surface lava pools.
		BiomeModifications.addFeature(
			BiomeSelectors.includeByKey(Biomes.END_HIGHLANDS, Biomes.END_MIDLANDS, Biomes.END_BARRENS, Biomes.SMALL_END_ISLANDS, VOID_ISLANDS),
			GenerationStep.Decoration.LAKES,
			VOID_LIQUID_POOL);
	}
}
