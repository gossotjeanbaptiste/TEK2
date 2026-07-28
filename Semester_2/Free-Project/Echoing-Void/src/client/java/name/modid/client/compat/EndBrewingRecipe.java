package name.modid.client.compat;

import net.minecraft.resources.Identifier;
import net.minecraft.world.item.ItemStack;

public record EndBrewingRecipe(ItemStack ingredient, ItemStack potionInput, ItemStack potionOutput, Identifier uid) {
}
