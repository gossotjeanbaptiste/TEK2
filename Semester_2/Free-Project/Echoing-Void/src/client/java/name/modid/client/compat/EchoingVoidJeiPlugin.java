package name.modid.client.compat;

import java.util.List;

import mezz.jei.api.IModPlugin;
import mezz.jei.api.JeiPlugin;
import mezz.jei.api.constants.RecipeTypes;
import mezz.jei.api.constants.VanillaTypes;
import mezz.jei.api.recipe.vanilla.IJeiBrewingRecipe;
import mezz.jei.api.registration.IRecipeCatalystRegistration;
import mezz.jei.api.registration.IRecipeCategoryRegistration;
import mezz.jei.api.registration.IRecipeRegistration;
import mezz.jei.api.runtime.IJeiRuntime;

import net.minecraft.core.component.DataComponents;
import net.minecraft.resources.Identifier;
import net.minecraft.world.item.ItemStack;
import net.minecraft.world.item.Items;
import net.minecraft.world.item.alchemy.PotionContents;
import net.minecraft.world.item.alchemy.Potions;

import name.modid.EchoingVoid;
import name.modid.blocks.ModBlocks;
import name.modid.item.ModItems;
import name.modid.item.ModPotions;

@JeiPlugin
public class EchoingVoidJeiPlugin implements IModPlugin {
	private static final Identifier PLUGIN_ID = EchoingVoid.id("jei_plugin");

	@Override
	public Identifier getPluginUid() {
		return PLUGIN_ID;
	}

	@Override
	public void registerCategories(IRecipeCategoryRegistration registration) {
		registration.addRecipeCategories(new EndBrewingRecipeCategory(registration.getJeiHelpers().getGuiHelper()));
	}

	@Override
	public void registerRecipes(IRecipeRegistration registration) {
		ItemStack awkwardPotion = new ItemStack(Items.POTION);
		awkwardPotion.set(DataComponents.POTION_CONTENTS, new PotionContents(Potions.AWKWARD));
		ItemStack voidPoisonedPotion = new ItemStack(ModItems.VOID_POISONED_POTION);
		ItemStack voidPoisonedSplashPotion = new ItemStack(ModItems.VOID_POISONED_SPLASH_POTION);

		// Only the End Brewing Stand can perform this step - shown in its own dedicated category.
		EndBrewingRecipe creationRecipe = new EndBrewingRecipe(
			new ItemStack(Items.ENDER_PEARL),
			awkwardPotion,
			voidPoisonedPotion,
			EchoingVoid.id("end_brewing_stand_void_poisoned")
		);
		registration.addRecipes(EndBrewingRecipeCategory.TYPE, List.of(creationRecipe));

		// Splash/Lingering conversion works in any brewing stand, like vanilla - shown in the shared Brewing category.
		IJeiBrewingRecipe splashRecipe = registration.getVanillaRecipeFactory().createBrewingRecipe(
			List.of(new ItemStack(Items.GUNPOWDER)),
			voidPoisonedPotion,
			voidPoisonedSplashPotion,
			EchoingVoid.id("void_poisoned_splash_potion")
		);
		IJeiBrewingRecipe lingeringRecipe = registration.getVanillaRecipeFactory().createBrewingRecipe(
			List.of(new ItemStack(Items.DRAGON_BREATH)),
			voidPoisonedSplashPotion,
			new ItemStack(ModItems.VOID_POISONED_LINGERING_POTION),
			EchoingVoid.id("void_poisoned_lingering_potion")
		);
		registration.addRecipes(RecipeTypes.BREWING, List.of(splashRecipe, lingeringRecipe));
	}

	@Override
	public void registerRecipeCatalysts(IRecipeCatalystRegistration registration) {
		registration.addCraftingStation(EndBrewingRecipeCategory.TYPE, ModBlocks.END_BREWING_STAND);
	}

	@Override
	public void onRuntimeAvailable(IJeiRuntime jeiRuntime) {
		// Registering ModPotions.VOID_POISONED globally (for tooltips/splash-lingering reuse) makes JEI's
		// own vanilla-potion scanner discover it on the generic minecraft:potion/splash_potion/lingering_potion
		// items too, even though our mod only ever produces its own dedicated items. Hide those phantom copies.
		ItemStack potion = new ItemStack(Items.POTION);
		potion.set(DataComponents.POTION_CONTENTS, new PotionContents(ModPotions.VOID_POISONED));
		ItemStack splashPotion = new ItemStack(Items.SPLASH_POTION);
		splashPotion.set(DataComponents.POTION_CONTENTS, new PotionContents(ModPotions.VOID_POISONED));
		ItemStack lingeringPotion = new ItemStack(Items.LINGERING_POTION);
		lingeringPotion.set(DataComponents.POTION_CONTENTS, new PotionContents(ModPotions.VOID_POISONED));
		ItemStack tippedArrow = new ItemStack(Items.TIPPED_ARROW);
		tippedArrow.set(DataComponents.POTION_CONTENTS, new PotionContents(ModPotions.VOID_POISONED));

		jeiRuntime.getIngredientManager()
			.removeIngredientsAtRuntime(VanillaTypes.ITEM_STACK, List.of(potion, splashPotion, lingeringPotion, tippedArrow));
	}
}
