/*
** EPITECH PROJECT, 2026
** GpuDraw.cpp
** File description:
** GPU-side drawing helpers: instanced models and GPU skinning
*/

#include "gui/GpuDraw.hpp"
#include <raymath.h>
#include <rlgl.h>
#include <cstring>
#include <string>

// Fourni par libGL (déjà liée) ; évite de dépendre des en-têtes GL.
extern "C" const unsigned char *glGetString(unsigned int name);

namespace {

// Emplacement de l'attribut de matrice par instance (mat4 = 4 emplacements
// consécutifs). raylib réserve 0 à 8 pour ses attributs par défaut.
constexpr int INSTANCE_LOC = 9;

// Taille du tableau de matrices d'os du shader de skinning (les personnages
// du jeu en ont 79).
constexpr int MAX_BONES = 128;

// Les shaders reproduisent le shader par défaut de raylib (texture x
// couleur du matériau x couleur du sommet, sans éclairage) : le rendu ne
// change pas, seul le chemin des sommets diffère.
const char *INSTANCING_VS = R"(#version 330
in vec3 vertexPosition;
in vec2 vertexTexCoord;
in vec4 vertexColor;
layout(location = 9) in mat4 instanceTransform;
uniform mat4 mvp;
out vec2 fragTexCoord;
out vec4 fragColor;
void main()
{
    fragTexCoord = vertexTexCoord;
    fragColor = vertexColor;
    gl_Position = mvp*instanceTransform*vec4(vertexPosition, 1.0);
}
)";

const char *SKINNING_VS = R"(#version 330
in vec3 vertexPosition;
in vec2 vertexTexCoord;
in vec4 vertexColor;
in vec4 vertexBoneIds;
in vec4 vertexBoneWeights;
uniform mat4 boneMatrices[128];
uniform mat4 mvp;
out vec2 fragTexCoord;
out vec4 fragColor;
void main()
{
    ivec4 ids = ivec4(vertexBoneIds);
    vec4 p = vec4(vertexPosition, 1.0);
    vec4 skinned = vertexBoneWeights.x*(boneMatrices[ids.x]*p)
                 + vertexBoneWeights.y*(boneMatrices[ids.y]*p)
                 + vertexBoneWeights.z*(boneMatrices[ids.z]*p)
                 + vertexBoneWeights.w*(boneMatrices[ids.w]*p);
    fragTexCoord = vertexTexCoord;
    fragColor = vertexColor;
    gl_Position = mvp*skinned;
}
)";

const char *BASIC_FS = R"(#version 330
in vec2 fragTexCoord;
in vec4 fragColor;
uniform sampler2D texture0;
uniform vec4 colDiffuse;
out vec4 finalColor;
void main()
{
    finalColor = texture(texture0, fragTexCoord)*colDiffuse*fragColor;
}
)";

constexpr unsigned int GL_RENDERER_ENUM = 0x1F01;

Shader      g_instancing = {};
Shader      g_skinning   = {};
bool        g_instOk     = false;
bool        g_skinOk     = false;
bool        g_software   = false;
std::string g_renderer;

bool isCustomShader(const Shader &s)
{
    return s.id > 0 && s.id != rlGetShaderIdDefault() && s.locs != nullptr;
}

void release(Shader &s, bool &ok)
{
    if (isCustomShader(s))
        UnloadShader(s);
    s  = {};
    ok = false;
}

Color tinted(Color c, Color t)
{
    return {static_cast<unsigned char>(c.r * t.r / 255),
            static_cast<unsigned char>(c.g * t.g / 255),
            static_cast<unsigned char>(c.b * t.b / 255),
            static_cast<unsigned char>(c.a * t.a / 255)};
}

bool hasCustomTexture(const Material &m)
{
    unsigned int id = m.maps[MATERIAL_MAP_DIFFUSE].texture.id;
    return id != 0 && id != rlGetTextureIdDefault();
}

// Repli sans instanciation : DrawModelEx avec une matrice déjà calculée.
void drawModelWithTransform(const Model &model, const Matrix &transform,
                            Color tint)
{
    for (int i = 0; i < model.meshCount; ++i) {
        Material &mat  = model.materials[model.meshMaterial[i]];
        Color     base = mat.maps[MATERIAL_MAP_DIFFUSE].color;
        mat.maps[MATERIAL_MAP_DIFFUSE].color = tinted(base, tint);
        DrawMesh(model.meshes[i], mat, transform);
        mat.maps[MATERIAL_MAP_DIFFUSE].color = base;
    }
}

}  // namespace

void GpuDraw::load()
{
    const unsigned char *name = glGetString(GL_RENDERER_ENUM);
    g_renderer = name ? reinterpret_cast<const char *>(name) : "?";
    g_software = g_renderer.find("llvmpipe") != std::string::npos
              || g_renderer.find("softpipe") != std::string::npos
              || g_renderer.find("Software") != std::string::npos;

    g_instancing = LoadShaderFromMemory(INSTANCING_VS, BASIC_FS);
    g_instOk = isCustomShader(g_instancing)
        && GetShaderLocationAttrib(g_instancing, "instanceTransform") == INSTANCE_LOC
        && GetShaderLocation(g_instancing, "mvp") >= 0;
    if (g_instOk)
        // DrawMeshInstanced lit l'emplacement de l'attribut d'instance ici.
        g_instancing.locs[SHADER_LOC_MATRIX_MODEL] = INSTANCE_LOC;
    else
        release(g_instancing, g_instOk);

    g_skinning = LoadShaderFromMemory(SKINNING_VS, BASIC_FS);
    g_skinOk = isCustomShader(g_skinning)
        && GetShaderLocation(g_skinning, "boneMatrices") >= 0
        && GetShaderLocationAttrib(g_skinning, "vertexBoneIds") >= 0
        && GetShaderLocationAttrib(g_skinning, "vertexBoneWeights") >= 0;
    if (g_skinOk) {
        g_skinning.locs[SHADER_LOC_BONE_MATRICES] =
            GetShaderLocation(g_skinning, "boneMatrices");
        g_skinning.locs[SHADER_LOC_VERTEX_BONEIDS] =
            GetShaderLocationAttrib(g_skinning, "vertexBoneIds");
        g_skinning.locs[SHADER_LOC_VERTEX_BONEWEIGHTS] =
            GetShaderLocationAttrib(g_skinning, "vertexBoneWeights");
    } else {
        release(g_skinning, g_skinOk);
    }
}

void GpuDraw::unload()
{
    release(g_instancing, g_instOk);
    release(g_skinning, g_skinOk);
}

bool GpuDraw::instancingAvailable() { return g_instOk; }
bool GpuDraw::skinningAvailable() { return g_skinOk; }
bool GpuDraw::softwareRenderer() { return g_software; }
const std::string &GpuDraw::rendererName() { return g_renderer; }

Matrix GpuDraw::modelTransform(const Model &model, Vector3 position,
                               float yawDeg, Vector3 scale)
{
    // Même ordre que DrawModelEx : échelle -> rotation -> translation,
    // appliqué après la transformation propre du modèle.
    Matrix m = MatrixScale(scale.x, scale.y, scale.z);
    if (yawDeg != 0.0f)
        m = MatrixMultiply(m, MatrixRotateY(yawDeg * DEG2RAD));
    m = MatrixMultiply(m, MatrixTranslate(position.x, position.y, position.z));
    return MatrixMultiply(model.transform, m);
}

void GpuDraw::drawModelInstanced(const Model &model, const Matrix *transforms,
                                 int count, Color tint)
{
    if (count <= 0)
        return;
    if (!g_instOk) {
        for (int k = 0; k < count; ++k)
            drawModelWithTransform(model, transforms[k], tint);
        return;
    }
    for (int i = 0; i < model.meshCount; ++i) {
        // Copie du matériau pour changer de shader ; le tableau maps est
        // partagé avec le modèle, d'où la restauration de la couleur.
        Material mat = model.materials[model.meshMaterial[i]];
        Color   base = mat.maps[MATERIAL_MAP_DIFFUSE].color;
        mat.shader = g_instancing;
        mat.maps[MATERIAL_MAP_DIFFUSE].color = tinted(base, tint);
        DrawMeshInstanced(model.meshes[i], mat, transforms, count);
        mat.maps[MATERIAL_MAP_DIFFUSE].color = base;

        // DrawMeshInstanced laisse l'attribut d'instance actif dans le VAO,
        // pointant vers un tampon qu'il vient de supprimer : on le coupe
        // pour que les DrawModel classiques de ce mesh restent sains.
        if (rlEnableVertexArray(model.meshes[i].vaoId)) {
            for (int a = 0; a < 4; ++a)
                rlDisableVertexAttribute(INSTANCE_LOC + a);
            rlDisableVertexArray();
        }
    }
}

bool GpuDraw::prepareSkinnedModel(Model &model)
{
    if (model.meshCount <= 0 || model.boneCount <= 0)
        return false;
    // En rendu logiciel (llvmpipe), le shader de skinning est plus lent que
    // le calcul CPU : on fusionne quand même, mais l'animation reste CPU.
    bool gpu = g_skinOk && !g_software && model.boneCount <= MAX_BONES;

    int vertexTotal = 0;
    int indexTotal  = 0;
    bool canMerge   = true;
    for (int i = 0; i < model.meshCount; ++i) {
        const Mesh &m = model.meshes[i];
        // Sans poids, le skinning GPU écraserait le mesh à l'origine (le
        // skinning CPU de raylib le laisse immobile) : on ne touche à rien.
        if (m.vertexCount <= 0 || m.vertices == nullptr || m.boneIds == nullptr
            || m.boneWeights == nullptr || m.boneMatrices == nullptr
            || m.boneCount != model.boneCount)
            return false;
        vertexTotal += m.vertexCount;
        indexTotal  += m.indices ? m.triangleCount * 3 : m.vertexCount;
        // Une texture ne survit pas à la fusion (une seule par matériau).
        canMerge = canMerge && !hasCustomTexture(model.materials[model.meshMaterial[i]]);
    }
    // Indices 16 bits dans raylib : 65535 sommets au plus par mesh.
    canMerge = canMerge && vertexTotal <= 65535 && model.meshCount > 1;

    if (!canMerge) {
        if (gpu)
            for (int i = 0; i < model.materialCount; ++i)
                model.materials[i].shader = g_skinning;
        return gpu;
    }

    // Un seul mesh pour tout le personnage : la couleur de chaque matériau
    // passe dans les sommets, ce qui fait un appel de dessin au lieu d'un
    // par morceau (tête, corps, vêtements...). Pas de normales : aucun
    // shader du jeu ne les utilise, et le skinning CPU de raylib inversait
    // une matrice par sommet et par os pour les recalculer.
    Mesh out = {};
    out.vertexCount   = vertexTotal;
    out.triangleCount = indexTotal / 3;
    out.vertices    = static_cast<float *>(MemAlloc(vertexTotal * 3 * sizeof(float)));
    out.texcoords   = static_cast<float *>(MemAlloc(vertexTotal * 2 * sizeof(float)));
    out.colors      = static_cast<unsigned char *>(MemAlloc(vertexTotal * 4));
    out.indices     = static_cast<unsigned short *>(MemAlloc(indexTotal * sizeof(unsigned short)));
    out.boneIds     = static_cast<unsigned char *>(MemAlloc(vertexTotal * 4));
    out.boneWeights = static_cast<float *>(MemAlloc(vertexTotal * 4 * sizeof(float)));
    out.boneCount    = model.boneCount;
    out.boneMatrices = static_cast<Matrix *>(MemAlloc(model.boneCount * sizeof(Matrix)));
    for (int b = 0; b < model.boneCount; ++b)
        out.boneMatrices[b] = MatrixIdentity();

    int vBase = 0;
    int iPos  = 0;
    for (int i = 0; i < model.meshCount; ++i) {
        const Mesh &m = model.meshes[i];
        int n = m.vertexCount;
        std::memcpy(out.vertices + vBase * 3, m.vertices, n * 3 * sizeof(float));
        if (m.texcoords)
            std::memcpy(out.texcoords + vBase * 2, m.texcoords, n * 2 * sizeof(float));
        std::memcpy(out.boneIds + vBase * 4, m.boneIds, n * 4);
        std::memcpy(out.boneWeights + vBase * 4, m.boneWeights, n * 4 * sizeof(float));

        Color mc = model.materials[model.meshMaterial[i]].maps[MATERIAL_MAP_DIFFUSE].color;
        for (int v = 0; v < n; ++v) {
            Color c = mc;
            if (m.colors)
                c = tinted({m.colors[v * 4], m.colors[v * 4 + 1],
                            m.colors[v * 4 + 2], m.colors[v * 4 + 3]}, mc);
            unsigned char *dst = out.colors + (vBase + v) * 4;
            dst[0] = c.r;
            dst[1] = c.g;
            dst[2] = c.b;
            dst[3] = c.a;
        }

        if (m.indices) {
            for (int k = 0; k < m.triangleCount * 3; ++k)
                out.indices[iPos++] = static_cast<unsigned short>(m.indices[k] + vBase);
        } else {
            for (int k = 0; k < n; ++k)
                out.indices[iPos++] = static_cast<unsigned short>(k + vBase);
        }
        vBase += n;
    }
    if (!gpu) {
        // Skinning CPU (UpdateModelAnimation) : raylib écrit la pose dans
        // animVertices puis met à jour le tampon de positions.
        out.animVertices = static_cast<float *>(MemAlloc(vertexTotal * 3 * sizeof(float)));
        std::memcpy(out.animVertices, out.vertices, vertexTotal * 3 * sizeof(float));
    }
    UploadMesh(&out, !gpu);

    // Remplace meshes et matériaux d'origine (libérés comme UnloadModel le
    // ferait) ; squelette et pose de liaison restent ceux du modèle.
    for (int i = 0; i < model.meshCount; ++i)
        UnloadMesh(model.meshes[i]);
    for (int i = 0; i < model.materialCount; ++i)
        UnloadMaterial(model.materials[i]);
    MemFree(model.meshes);
    MemFree(model.materials);
    MemFree(model.meshMaterial);

    model.meshCount     = 1;
    model.meshes        = static_cast<Mesh *>(MemAlloc(sizeof(Mesh)));
    model.meshes[0]     = out;
    model.materialCount = 1;
    model.materials     = static_cast<Material *>(MemAlloc(sizeof(Material)));
    model.materials[0]  = LoadMaterialDefault();
    if (gpu)
        model.materials[0].shader = g_skinning;
    model.meshMaterial    = static_cast<int *>(MemAlloc(sizeof(int)));
    model.meshMaterial[0] = 0;
    return gpu;
}
