package name.modid;

import net.fabricmc.api.ModInitializer;

import net.minecraft.resources.Identifier;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import name.modid.block.entity.ModBlockEntities;
import name.modid.blocks.ModBlocks;
import name.modid.effect.ModMobEffects;
import name.modid.entity.BlastlingSpawner;
import name.modid.entity.ModEntities;
import name.modid.entity.WatchlingSpawner;
import name.modid.fluid.ModFluids;
import name.modid.item.ModItems;
import name.modid.item.ModPotions;
import name.modid.loot.ModLootTables;
import name.modid.sound.ModSounds;
import name.modid.worldgen.ModFeatures;
import name.modid.worldgen.ModStructureProcessors;
import name.modid.worldgen.ModWorldGen;

public class EchoingVoid implements ModInitializer {
	public static final String MOD_ID = "echoing_void";

	// This logger is used to write text to the console and the log file.
	// It is considered best practice to use your mod id as the logger's name.
	// That way, it's clear which mod wrote info, warnings, and errors.
	public static final Logger LOGGER = LoggerFactory.getLogger(MOD_ID);

	@Override
	public void onInitialize() {
		// This code runs as soon as Minecraft is in a mod-load-ready state.
		// However, some things (like resources) may still be uninitialized.
		// Proceed with mild caution.

		LOGGER.info("Hello Fabric world!");

		ModSounds.init();
		ModMobEffects.init();
		ModPotions.init();
		ModFluids.init();
		ModItems.init();
		ModBlocks.init();
		ModCreativeModeTabs.init();
		ModBlockEntities.init();
		ModStructureProcessors.init();
		ModFeatures.init();
		ModWorldGen.init();
		ModLootTables.init();
		ModEntities.init();
		WatchlingSpawner.init();
		BlastlingSpawner.init();
	}

	public static Identifier id(String path) {
		return Identifier.fromNamespaceAndPath(MOD_ID, path);
	}
}
