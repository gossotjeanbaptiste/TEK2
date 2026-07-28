package name.modid.tag;

import net.minecraft.core.registries.Registries;
import net.minecraft.tags.TagKey;
import net.minecraft.world.item.Item;

import name.modid.EchoingVoid;

public class ModItemTags {
	// Anvil repair material for the enderite tool/armor line and the two legendary weapons -
	// populated with just the enderite ingot.
	public static final TagKey<Item> ENDERITE_REPAIR =
		TagKey.create(Registries.ITEM, EchoingVoid.id("enderite_repair"));
}
