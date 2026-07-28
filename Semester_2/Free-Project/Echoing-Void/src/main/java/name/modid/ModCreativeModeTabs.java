package name.modid;

import net.fabricmc.fabric.api.creativetab.v1.FabricCreativeModeTab;

import net.minecraft.core.Registry;
import net.minecraft.core.registries.BuiltInRegistries;
import net.minecraft.core.registries.Registries;
import net.minecraft.network.chat.Component;
import net.minecraft.resources.ResourceKey;
import net.minecraft.world.item.CreativeModeTab;
import net.minecraft.world.item.ItemStack;

import name.modid.blocks.ModBlocks;
import name.modid.item.ModItems;

public class ModCreativeModeTabs {
	public static final ResourceKey<CreativeModeTab> ECHOING_VOID_KEY =
		ResourceKey.create(Registries.CREATIVE_MODE_TAB, EchoingVoid.id("echoing_void"));

	public static final CreativeModeTab ECHOING_VOID = FabricCreativeModeTab.builder()
		.title(Component.translatable("itemGroup.echoing_void.echoing_void"))
		.icon(() -> new ItemStack(ModBlocks.VOID_STONE))
		.displayItems((parameters, output) -> {
			output.accept(ModBlocks.VOID_STONE);
			output.accept(ModBlocks.END_BREWING_STAND);
			output.accept(ModBlocks.ENDERITE_DEBRIS);
			output.accept(ModBlocks.ENDERITE_BLOCK);
			output.accept(ModItems.VOID_POISONED_POTION);
			output.accept(ModItems.VOID_POISONED_SPLASH_POTION);
			output.accept(ModItems.VOID_POISONED_LINGERING_POTION);
			output.accept(ModItems.ENDERITE_SCRAP);
			output.accept(ModItems.ENDERITE_INGOT);
			output.accept(ModItems.ENDERITE_UPGRADE_SMITHING_TEMPLATE);
			output.accept(ModItems.THE_BEGINNING_OF_THE_END);
			output.accept(ModItems.CALL_OF_THE_VOID);
			output.accept(ModItems.ENDERITE_SPEAR);
			output.accept(ModItems.ENDERITE_PICKAXE);
			output.accept(ModItems.ENDERITE_AXE);
			output.accept(ModItems.ENDERITE_SHOVEL);
			output.accept(ModItems.ENDERITE_HOE);
			output.accept(ModItems.ENDERITE_HELMET);
			output.accept(ModItems.ENDERITE_CHESTPLATE);
			output.accept(ModItems.ENDERITE_LEGGINGS);
			output.accept(ModItems.ENDERITE_BOOTS);
			output.accept(ModItems.WATCHLING_SPAWN_EGG);
			output.accept(ModItems.BLASTLING_SPAWN_EGG);
		})
		.build();

	public static void init() {
		Registry.register(BuiltInRegistries.CREATIVE_MODE_TAB, ECHOING_VOID_KEY, ECHOING_VOID);
	}
}
