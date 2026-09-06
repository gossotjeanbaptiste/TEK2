package name.modid.client.render.sky;

import java.lang.reflect.Method;

import com.mojang.blaze3d.pipeline.RenderPipeline;

import name.modid.EchoingVoid;

/**
 * Iris intercepts every RenderPipeline it recognizes and reroutes it through its own shader
 * program slots - even with no shaderpack selected. A pipeline it doesn't recognize (like ours)
 * is otherwise left unhandled and never draws. Iris exposes a small, stable "v0" API for mods to
 * register their custom pipelines against one of its program slots (see Nuit's own IrisCompat,
 * which this mirrors); pure reflection avoids a hard compile-time dependency on Iris.
 */
public final class IrisCompat {
	private static final int MIN_API_REVISION_FOR_PIPELINE_ASSIGNMENT = 3;

	private static Object apiInstance;
	private static Method assignPipelineMethod;
	private static Object skyTexturedProgram;

	public static boolean canAssignPipelines() {
		return assignPipelineMethod != null;
	}

	public static void assignSkyTexturedPipeline(RenderPipeline pipeline) {
		if (!canAssignPipelines()) {
			return;
		}

		try {
			assignPipelineMethod.invoke(apiInstance, pipeline, skyTexturedProgram);
		} catch (ReflectiveOperationException exception) {
			EchoingVoid.LOGGER.debug("Failed to register {} with Iris", pipeline.getLocation(), exception);
		}
	}

	static {
		try {
			Class<?> api = Class.forName("net.irisshaders.iris.api.v0.IrisApi");
			apiInstance = api.cast(api.getMethod("getInstance").invoke(null));

			int minorApiRevision = (Integer) api.getMethod("getMinorApiRevision").invoke(apiInstance);
			if (minorApiRevision >= MIN_API_REVISION_FOR_PIPELINE_ASSIGNMENT) {
				Class<?> irisProgram = Class.forName("net.irisshaders.iris.api.v0.IrisProgram");
				assignPipelineMethod = api.getMethod("assignPipeline", RenderPipeline.class, irisProgram);
				skyTexturedProgram = Enum.valueOf(irisProgram.asSubclass(Enum.class), "SKY_TEXTURED");
			}
		} catch (ReflectiveOperationException | LinkageError | RuntimeException exception) {
			// Iris isn't present, or its API shape changed - leave assignPipelineMethod null so
			// canAssignPipelines() reports false and we quietly skip pipeline registration.
		}
	}

	private IrisCompat() {
	}
}
