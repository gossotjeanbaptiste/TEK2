package name.modid.tag;

import net.minecraft.core.registries.Registries;
import net.minecraft.tags.TagKey;
import net.minecraft.world.entity.EntityType;

import name.modid.EchoingVoid;

public class ModEntityTypeTags {
	public static final TagKey<EntityType<?>> IMMUNE_TO_VOID_POISONED =
		TagKey.create(Registries.ENTITY_TYPE, EchoingVoid.id("immune_to_void_poisoned"));
}
