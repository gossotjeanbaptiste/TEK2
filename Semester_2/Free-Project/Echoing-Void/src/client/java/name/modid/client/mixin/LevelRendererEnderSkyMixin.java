package name.modid.client.mixin;

import org.spongepowered.asm.mixin.Mixin;
import org.spongepowered.asm.mixin.injection.At;
import org.spongepowered.asm.mixin.injection.Inject;
import org.spongepowered.asm.mixin.injection.callback.CallbackInfo;

import com.mojang.blaze3d.buffers.GpuBufferSlice;

import net.minecraft.client.renderer.LevelRenderer;
import net.minecraft.client.renderer.SkyRenderer;
import net.minecraft.client.renderer.state.level.SkyRenderState;
import net.minecraft.world.level.dimension.DimensionType;

import name.modid.EchoingVoid;
import name.modid.client.render.sky.EnderSkyEffectsRenderer;

@Mixin(LevelRenderer.class)
public abstract class LevelRendererEnderSkyMixin {
	// The lambda has two independent branches (End vs. overworld sky), each ending in its own
	// RETURN - TAIL only matches the last one, so it silently never fires for the End branch.
	// RETURN matches both, and the skybox check below keeps this a no-op on the other branch.
	@Inject(
		method = "lambda$addSkyPass$0(Lcom/mojang/blaze3d/buffers/GpuBufferSlice;Lnet/minecraft/client/renderer/state/level/SkyRenderState;Lnet/minecraft/client/renderer/SkyRenderer;)V",
		at = @At("RETURN")
	)
	private static void echoingVoid$renderEnderSkyEffects(
		GpuBufferSlice fogParameters, SkyRenderState skyRenderState, SkyRenderer skyRenderer, CallbackInfo ci
	) {
		if (skyRenderState.skybox == DimensionType.Skybox.END) {
			try {
				EnderSkyEffectsRenderer.render();
			} catch (Throwable t) {
				EchoingVoid.LOGGER.error("Failed to render ender sky effects", t);
			}
		}
	}
}
