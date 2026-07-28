package name.modid.item;
import net.minecraft.world.item.ItemStack;
import net.minecraft.world.item.PotionItem;

public class VoidPoisonedPotionItem extends PotionItem {
	public VoidPoisonedPotionItem(Properties properties) {
		super(properties);
	}

	@Override
	public ItemStack getDefaultInstance() {
		// PotionItem.getDefaultInstance() unconditionally stamps a Potions.WATER PotionContents,
		// which would override the Void Poisoned contents already baked into this item's properties.
		return new ItemStack(this);
	}
}
