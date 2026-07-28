package name.modid.client.compat;

import mezz.jei.api.gui.ITickTimer;
import mezz.jei.api.gui.builder.IRecipeLayoutBuilder;
import mezz.jei.api.gui.drawable.IDrawable;
import mezz.jei.api.gui.drawable.IDrawableAnimated;
import mezz.jei.api.gui.drawable.IDrawableAnimated.StartDirection;
import mezz.jei.api.gui.drawable.IDrawableStatic;
import mezz.jei.api.gui.ingredient.IRecipeSlotsView;
import mezz.jei.api.helpers.IGuiHelper;
import mezz.jei.api.recipe.IFocusGroup;
import mezz.jei.api.recipe.category.AbstractRecipeCategory;
import mezz.jei.api.recipe.types.IRecipeType;

import net.minecraft.client.gui.GuiGraphicsExtractor;
import net.minecraft.network.chat.Component;
import net.minecraft.resources.Identifier;

import name.modid.EchoingVoid;
import name.modid.blocks.ModBlocks;

public class EndBrewingRecipeCategory extends AbstractRecipeCategory<EndBrewingRecipe> {
	public static final IRecipeType<EndBrewingRecipe> TYPE =
		IRecipeType.create(EchoingVoid.MOD_ID, "end_brewing", EndBrewingRecipe.class);

	private static final Identifier BREWING_STAND_GUI = Identifier.withDefaultNamespace("textures/gui/container/brewing_stand.png");
	private static final Identifier ARROW_SPRITE = Identifier.withDefaultNamespace("textures/gui/sprites/container/brewing_stand/brew_progress.png");
	private static final Identifier BUBBLES_SPRITE = Identifier.withDefaultNamespace("textures/gui/sprites/container/brewing_stand/bubbles.png");

	private final IDrawable background;
	private final IDrawableAnimated arrow;
	private final IDrawableAnimated bubbles;

	public EndBrewingRecipeCategory(IGuiHelper guiHelper) {
		super(TYPE, Component.translatable("gui.echoing_void.category.end_brewing"),
			guiHelper.createDrawableItemLike(ModBlocks.END_BREWING_STAND), 155, 76);
		this.background = guiHelper.drawableBuilder(BREWING_STAND_GUI, 0, 0, 130, 76).setTextureSize(256, 256).build();
		IDrawableStatic arrowStatic = guiHelper.createDrawable(ARROW_SPRITE, 0, 0, 9, 28);
		this.arrow = guiHelper.createAnimatedDrawable(arrowStatic, guiHelper.createTickTimer(400, 28, false), StartDirection.TOP);
		IDrawableStatic bubblesStatic = guiHelper.createDrawable(BUBBLES_SPRITE, 0, 0, 12, 29);
		this.bubbles = guiHelper.createAnimatedDrawable(bubblesStatic, new BubblesTickTimer(guiHelper), StartDirection.BOTTOM);
	}

	@Override
	public void draw(EndBrewingRecipe recipe, IRecipeSlotsView recipeSlotsView, GuiGraphicsExtractor guiGraphics, double mouseX, double mouseY) {
		this.background.draw(guiGraphics, 0, 0);
		this.bubbles.draw(guiGraphics, 63, 14);
		this.arrow.draw(guiGraphics, 97, 16);
	}

	@Override
	public void setRecipe(IRecipeLayoutBuilder builder, EndBrewingRecipe recipe, IFocusGroup focuses) {
		builder.addInputSlot(56, 51).add(recipe.potionInput());
		builder.addInputSlot(79, 58).add(recipe.potionInput());
		builder.addInputSlot(102, 51).add(recipe.potionInput());
		builder.addInputSlot(79, 17).add(recipe.ingredient());
		builder.addOutputSlot(136, 30).add(recipe.potionOutput()).setStandardSlotBackground();
	}

	@Override
	public Identifier getIdentifier(EndBrewingRecipe recipe) {
		return recipe.uid();
	}

	// Mirrors BrewingStandScreen.BUBBLELENGTHS: a stepped (not smooth) countdown, 2 game ticks per step.
	private static class BubblesTickTimer implements ITickTimer {
		private static final int[] BUBBLE_LENGTHS = {29, 24, 20, 16, 11, 6, 0};

		private final ITickTimer internalTimer;

		BubblesTickTimer(IGuiHelper guiHelper) {
			this.internalTimer = guiHelper.createTickTimer(14, BUBBLE_LENGTHS.length - 1, false);
		}

		@Override
		public int getValue() {
			return BUBBLE_LENGTHS[this.internalTimer.getValue()];
		}

		@Override
		public int getMaxValue() {
			return BUBBLE_LENGTHS[0];
		}
	}
}
