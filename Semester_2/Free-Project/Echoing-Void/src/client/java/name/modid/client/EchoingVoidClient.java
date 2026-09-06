package name.modid.client;

import net.fabricmc.api.ClientModInitializer;
import net.fabricmc.fabric.api.client.networking.v1.ClientPlayConnectionEvents;
import net.fabricmc.fabric.api.client.render.fluid.v1.FluidRenderingRegistry;
import net.fabricmc.fabric.api.client.rendering.v1.ModelLayerRegistry;

import net.minecraft.ChatFormatting;
import net.minecraft.client.renderer.block.FluidModel;
import net.minecraft.client.renderer.entity.EntityRenderers;
import net.minecraft.client.renderer.entity.ThrownItemRenderer;
import net.minecraft.client.resources.model.sprite.Material;
import net.minecraft.network.chat.Component;

import name.modid.EchoingVoid;
import name.modid.client.render.BlastlingFlameModel;
import name.modid.client.render.BlastlingModel;
import name.modid.client.render.BlastlingRenderer;
import name.modid.client.render.WatchlingModel;
import name.modid.client.render.WatchlingRenderer;
import name.modid.entity.ModEntities;
import name.modid.fluid.ModFluids;

public class EchoingVoidClient implements ClientModInitializer {
	@Override
	public void onInitializeClient() {
		// This entrypoint is suitable for setting up client-specific logic, such as rendering.
		ModelLayerRegistry.registerModelLayer(WatchlingRenderer.LAYER, WatchlingModel::createBodyLayer);
		EntityRenderers.register(ModEntities.WATCHLING, WatchlingRenderer::new);

		ModelLayerRegistry.registerModelLayer(BlastlingRenderer.LAYER, BlastlingModel::createBodyLayer);
		ModelLayerRegistry.registerModelLayer(BlastlingRenderer.FLAME_LAYER, BlastlingFlameModel::createBodyLayer);
		EntityRenderers.register(ModEntities.BLASTLING, BlastlingRenderer::new);
		// Full-bright flat billboard, like a snowball/pearl - it's magic goo, not a physical object.
		EntityRenderers.register(ModEntities.BLASTLING_GOO, context -> new ThrownItemRenderer<>(context, 0.9F, true));

		// Texture already carries its own dark purple color, so no biome-style tint (unlike water).
		FluidRenderingRegistry.register(ModFluids.VOID_LIQUID, ModFluids.FLOWING_VOID_LIQUID,
			new FluidModel.Unbaked(
				new Material(EchoingVoid.id("block/void_liquid_still")),
				new Material(EchoingVoid.id("block/void_liquid_flow")),
				new Material(EchoingVoid.id("block/void_liquid_overlay")),
				null));

		ClientPlayConnectionEvents.JOIN.register((handler, sender, client) -> {
			if (client.player != null) {
				Component prefix = Component.literal("Echoing Void").withStyle(ChatFormatting.DARK_PURPLE, ChatFormatting.BOLD);
				Component nuit = Component.literal("Nuit").withStyle(ChatFormatting.LIGHT_PURPLE);
				Component tip = Component.translatable("message.echoing_void.nuit_performance_tip", nuit).withStyle(ChatFormatting.GRAY);
				client.player.sendSystemMessage(
					prefix.copy().append(Component.literal(": ").withStyle(ChatFormatting.DARK_GRAY)).append(tip));
			}
		});
	}
}