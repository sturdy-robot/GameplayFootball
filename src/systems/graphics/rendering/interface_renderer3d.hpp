// written by bastiaan konings schuiling 2008 - 2014
// this work is public domain. the code is undocumented, scruffy, untested, and
// should generally not be used for anything important. i do not offer support,
// so don't ask. to be used for inspiration :)

#ifndef _HPP_RENDERER3D
#define _HPP_RENDERER3D

#include "types/resource.hpp"
#include "types/thread.hpp"

#include "base/math/vector3.hpp"
#include "base/sdl_surface.hpp"

#include "../resources/vertexbuffer.hpp"

#include "base/geometry/triangle.hpp"

#include "types/material.hpp"

namespace blunted {

class Camera;
class Geometry;
class Texture;

struct Overlay2DQueueEntry {
  boost::intrusive_ptr<Resource<Texture>> texture;
  int position[2];
  int size[2];
};

struct Renderer3DMaterial {
  boost::intrusive_ptr<Resource<Texture>> diffuseTexture;
  boost::intrusive_ptr<Resource<Texture>> normalTexture;
  boost::intrusive_ptr<Resource<Texture>> specularTexture;
  boost::intrusive_ptr<Resource<Texture>> illuminationTexture;
  float shininess;
  float specular_amount;
  Vector3 self_illumination;
};

struct VertexBufferIndex {
  int startIndex;
  int size;
  Renderer3DMaterial material;
};

struct VertexBufferQueueEntry {
  std::deque<VertexBufferIndex> vertexBufferIndices;
  boost::intrusive_ptr<Resource<VertexBuffer>> vertexBuffer;
  AABB aabb;
  Vector3 position;
  Quaternion rotation;
  // todo: think about this: int id_cache; // for quick access - getting
  // resource's id needs mutexing and all
};

struct ShadowMap {
  std::string cameraName;
  Matrix4 lightProjectionMatrix;
  Matrix4 lightViewMatrix;
  std::deque<VertexBufferQueueEntry> visibleGeometry;
  boost::intrusive_ptr<Resource<Texture>> texture;
  int frameBufferID;
};

enum e_ViewRenderTarget {
  e_ViewRenderTarget_Texture,
  e_ViewRenderTarget_Context
};

struct View {
  e_ViewRenderTarget target;
  int targetTexID;
  int x, y, width, height;

  int gBufferID;
  int gBuffer_DepthTexID;
  int gBuffer_AlbedoTexID;
  int gBuffer_NormalTexID;
  int gBuffer_AuxTexID;

  int accumBufferID;
  int accumBuffer_AccumTexID;
  int accumBuffer_ModifierTexID;
};

struct LightQueueEntry {
  bool hasShadow;
  Matrix4 lightProjectionMatrix;
  Matrix4 lightViewMatrix;
  boost::intrusive_ptr<Resource<Texture>> shadowMapTexture;
  Vector3 position;
  int type; // 0 == directional, 1 == point
  Vector3 color;
  float radius;
  bool shadow;
  AABB aabb;
};

enum e_MatrixMode { e_MatrixMode_Projection, e_MatrixMode_ModelView };

enum e_CullingMode {
  e_CullingMode_Off,
  e_CullingMode_Front,
  e_CullingMode_Back
};

enum e_BlendingMode { e_BlendingMode_Off, e_BlendingMode_On };

enum e_BlendingFunction { e_BlendingFunction_Zero, e_BlendingFunction_One };

enum e_DepthFunction {
  e_DepthFunction_Never,
  e_DepthFunction_Equal,
  e_DepthFunction_Greater,
  e_DepthFunction_Less,
  e_DepthFunction_GreaterOrEqual,
  e_DepthFunction_LessOrEqual,
  e_DepthFunction_NotEqual,
  e_DepthFunction_Always
};

enum e_TextureMode { e_TextureMode_Off, e_TextureMode_2D };

enum e_TargetAttachment {
  e_TargetAttachment_None,
  e_TargetAttachment_Front,
  e_TargetAttachment_Back,
  e_TargetAttachment_Depth,   // can not be used with drawbuffers
  e_TargetAttachment_Stencil, // can not be used with drawbuffers
  e_TargetAttachment_Color0,
  e_TargetAttachment_Color1,
  e_TargetAttachment_Color2,
  e_TargetAttachment_Color3
};

enum e_PixelFormat {
  e_PixelFormat_Alpha,
  e_PixelFormat_RGB,
  e_PixelFormat_RGBA,
  e_PixelFormat_DepthComponent,
  e_PixelFormat_Luminance
};

enum e_InternalPixelFormat {
  //    e_InternalPixelFormat_RGB,
  e_InternalPixelFormat_RGB8,
  e_InternalPixelFormat_SRGB8,
  e_InternalPixelFormat_RGB16,
  //    e_InternalPixelFormat_RGBA,
  e_InternalPixelFormat_RGBA8,
  e_InternalPixelFormat_SRGBA8,
  e_InternalPixelFormat_RGBA16,
  e_InternalPixelFormat_RGBA16F,
  e_InternalPixelFormat_RGBA32F,

  e_InternalPixelFormat_RGBA4,
  e_InternalPixelFormat_RGB5_A1,
  e_InternalPixelFormat_DepthComponent,
  e_InternalPixelFormat_DepthComponent16,
  e_InternalPixelFormat_DepthComponent24,
  e_InternalPixelFormat_DepthComponent32,
  e_InternalPixelFormat_DepthComponent32F,
  e_InternalPixelFormat_StencilIndex8
};

enum e_VertexBufferUsage {
  e_VertexBufferUsage_StreamDraw,
  e_VertexBufferUsage_StreamRead,
  e_VertexBufferUsage_StreamCopy,
  e_VertexBufferUsage_StaticDraw,
  e_VertexBufferUsage_StaticRead,
  e_VertexBufferUsage_StaticCopy,
  e_VertexBufferUsage_DynamicDraw,
  e_VertexBufferUsage_DynamicRead,
  e_VertexBufferUsage_DynamicCopy
};

enum e_RenderMode {
  e_RenderMode_GeometryOnly,
  e_RenderMode_Diffuse,
  e_RenderMode_Full
};

struct Shader {
  std::string name;
  unsigned int programID;
  unsigned int vertexShaderID;
  unsigned int fragmentShaderID;
};

class Renderer3D : public Thread {

public:
  virtual ~Renderer3D() {};

  virtual void SwapBuffers() = 0;

  virtual void SetMatrix(const std::string &shaderUniformName,
                         const Matrix4 &matrix) = 0;

  virtual void
  RenderOverlay2D(const std::vector<Overlay2DQueueEntry> &overlay2DQueue) = 0;
  virtual void RenderOverlay2D() = 0;
  virtual void RenderLights(std::deque<LightQueueEntry> &lightQueue,
                            const Matrix4 &projectionMatrix,
                            const Matrix4 &viewMatrix) = 0;

  // --- new & improved

  // init & exit
  virtual bool CreateContext(int width, int height, int bpp,
                             bool fullscreen) = 0;
  virtual void Exit() = 0;

  virtual int CreateView(float x_percent, float y_percent, float width_percent,
                         float height_percent) = 0;
  virtual View &GetView(int viewID) = 0;
  virtual void DeleteView(int viewID) = 0;

  // general
  virtual void SetCullingMode(e_CullingMode cullingMode) = 0;
  virtual void SetBlendingMode(e_BlendingMode blendingMode) = 0;
  virtual void SetDepthFunction(e_DepthFunction depthFunction) = 0;
  virtual void SetDepthTesting(bool OnOff) = 0;
  virtual void SetDepthMask(bool OnOff) = 0;
  virtual void SetBlendingFunction(e_BlendingFunction blendingFunction1,
                                   e_BlendingFunction blendingFunction2) = 0;
  virtual void SetTextureMode(e_TextureMode textureMode) = 0;
  virtual void SetColor(const Vector3 &color, float alpha) = 0;
  virtual void SetColorMask(bool r, bool g, bool b, bool alpha) = 0;

  virtual void ClearBuffer(const Vector3 &color, bool clearDepth,
                           bool clearColor) = 0;

  virtual Matrix4 CreatePerspectiveMatrix(float aspectRatio, float nearCap = -1,
                                          float farCap = -1) = 0;
  virtual Matrix4 CreateOrthoMatrix(float left, float right, float bottom,
                                    float top, float nearCap = -1,
                                    float farCap = -1) = 0;

  // vertex buffers
  virtual VertexBufferID CreateVertexBuffer(float *vertices,
                                            unsigned int verticesDataSize,
                                            std::vector<unsigned int> indices,
                                            e_VertexBufferUsage usage) = 0;
  virtual void UpdateVertexBuffer(VertexBufferID vertexBufferID,
                                  float *vertices,
                                  unsigned int verticesDataSize) = 0;
  virtual void DeleteVertexBuffer(VertexBufferID vertexBufferID) = 0;
  virtual void RenderVertexBuffer(
      const std::deque<VertexBufferQueueEntry> &vertexBufferQueue,
      e_RenderMode renderMode = e_RenderMode_Full) = 0;
  virtual void
  RenderAABB(std::list<VertexBufferQueueEntry> &vertexBufferQueue) = 0;
  virtual void RenderAABB(std::list<LightQueueEntry> &lightQueue) = 0;

  // lights
  virtual void SetLight(const Vector3 &position, const Vector3 &color,
                        float radius) = 0;

  // textures
  virtual int CreateTexture(e_InternalPixelFormat internalPixelFormat,
                            e_PixelFormat pixelFormat, int width, int height,
                            bool alpha = false, bool repeat = true,
                            bool mipmaps = true, bool filter = true,
                            bool multisample = false,
                            bool compareDepth = false) = 0;
  virtual void ResizeTexture(int textureID, SDL_Surface *source,
                             e_InternalPixelFormat internalPixelFormat,
                             e_PixelFormat pixelFormat, bool alpha = false,
                             bool mipmaps = true) = 0;
  virtual void UpdateTexture(int textureID, SDL_Surface *source,
                             bool alpha = false, bool mipmaps = true) = 0;
  virtual void DeleteTexture(int textureID) = 0;
  virtual void CopyFrameBufferToTexture(int textureID, int width,
                                        int height) = 0;
  virtual void BindTexture(int textureID) = 0;
  virtual void SetTextureUnit(int textureUnit) = 0;
  virtual void SetClientTextureUnit(int textureUnit) = 0;

  // frame buffer
  virtual int CreateFrameBuffer() = 0;
  virtual void DeleteFrameBuffer(int fbID) = 0;
  virtual void BindFrameBuffer(int fbID) = 0;
  virtual void SetFrameBufferRenderBuffer(e_TargetAttachment targetAttachment,
                                          int rbID) = 0;
  virtual void SetFrameBufferTexture2D(e_TargetAttachment targetAttachment,
                                       int texID) = 0;
  virtual bool CheckFrameBufferStatus() = 0;
  virtual void SetFramebufferGammaCorrection(bool onOff) = 0;

  // render buffers
  virtual int CreateRenderBuffer() = 0;
  virtual void DeleteRenderBuffer(int rbID) = 0;
  virtual void BindRenderBuffer(int rbID) = 0;
  virtual void SetRenderBufferStorage(e_InternalPixelFormat internalPixelFormat,
                                      int width, int height) = 0;

  // render targets
  virtual void
  SetRenderTargets(std::vector<e_TargetAttachment> targetAttachments) = 0;

  // utility
  virtual void SetFOV(float angle) = 0;
  virtual void PushAttribute(int attr) = 0;
  virtual void PopAttribute() = 0;
  virtual void SetViewport(int x, int y, int width, int height) = 0;
  virtual void GetContextSize(int &width, int &height, int &bpp) = 0;
  virtual void SetPolygonOffset(float scale, float bias) = 0;

  // shaders
  virtual void LoadShader(const std::string &name,
                          const std::string &filename) = 0;
  virtual void UseShader(const std::string &name) = 0;
  virtual void SetUniformInt(const std::string &shaderName,
                             const std::string &varName, int value) = 0;
  virtual void SetUniformFloat(const std::string &shaderName,
                               const std::string &varName, float value) = 0;
  virtual void SetUniformFloat2(const std::string &shaderName,
                                const std::string &varName, float value1,
                                float value2) = 0;
  virtual void SetUniformFloat3(const std::string &shaderName,
                                const std::string &varName, float value1,
                                float value2, float value3) = 0;
  virtual void SetUniformFloat3Array(const std::string &shaderName,
                                     const std::string &varName, int count,
                                     float *values) = 0;
  virtual void SetUniformMatrix4(const std::string &shaderName,
                                 const std::string &varName,
                                 const Matrix4 &mat) = 0;

  virtual void HDRCaptureOverallBrightness() = 0;
  virtual float HDRGetOverallBrightness() = 0;

  void operator()() = 0;

protected:
  std::map<std::string, Shader> shaders;
  std::map<std::string, Shader>::iterator currentShader;
  std::vector<View> views;
};

} // namespace blunted

#endif
