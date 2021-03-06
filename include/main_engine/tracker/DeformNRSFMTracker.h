#ifndef _DEFORMNRSFM_TRACKER_H
#define _DEFORMNRSFM_TRACKER_H

#include "./TrackingEngine.h"
#include "./ImagePyramid.h"
#include "./FeaturePyramid.h"
#include "./OptimizationStrategy.h"
#include "./residual.h"
#include "./ProblemWrapper.h"

#include "../utils/qx_basic.h"
#include "../utils/qx_highlight_removal_bf.h"

#include "ceres/ceres.h"

// baType mapBA(std::string const& inString);

// dataTermErrorType mapErrorType(std::string const& inString);

// // Linear Solver mapping
// ceres::LinearSolverType mapLinearSolver(std::string const& inString);

class DeformNRSFMTracker : public TrackingEngine
{

public:

  DeformNRSFMTracker(TrackerSettings& settings, int width, int height, double K[3][3],
                     int startFrame, int numTrackingFrames);

  virtual ~DeformNRSFMTracker();

  bool setCurrentFrame(int curFrame);  // only useful when loading stuff
  void setIntrinsicMatrix(double K[3][3]); // set camera parameters
  void initializeCamera();

  void setInitialMesh(PangaeaMeshData& mesh){};
  void setInitialMeshPyramid(PangaeaMeshPyramid& initMeshPyramid);

  //void trackerInitSetup(TrackerOutputInfo& outputInfo);

  void loadGTMeshFromFile(int nFrame);
  void initializeGT();
  void updateGT();

  bool trackFrame(int nFrame, unsigned char* pColorImageRGB,
                  TrackerOutputInfo** pOutputInfo);

  void updateRenderingLevel(TrackerOutputInfo** pOutputInfo,
                            int nRenderLevel, bool renderType = false);

  void AddPhotometricCost(ceres::Problem& problem,
                          ceres::LossFunction* loss_function,
                          dataTermErrorType errorType);

  void AddPhotometricCostNew(ceres::Problem& problem,
                          ceres::LossFunction* loss_function,
                          dataTermErrorType errorType);

  void AddFeatureCost(ceres::Problem& problem,
                      ceres::LossFunction* loss_function);

  void AddTotalVariationCost(ceres::Problem& problem,
                             ceres::LossFunction* loss_function);
  void AddRotTotalVariationCost(ceres::Problem& problem,
                                ceres::LossFunction* loss_function);

  void AddARAPCost(ceres::Problem& problem,
                   ceres::LossFunction* loss_function);
  void AddInextentCost(ceres::Problem& problem,
                       ceres::LossFunction* loss_function);
  void AddDeformationCost(ceres::Problem& problem,
                          ceres::LossFunction* loss_function);
  void AddTemporalMotionCost(ceres::Problem& problem,
                             double rotWeight, double transWeight);
  void AddSmoothingCost(ceres::Problem& problem,
	  ceres::LossFunction* loss_function);
  void AddTemporalSHCoeffCost(ceres::Problem& problem,
    ceres::LossFunction* loss_function);
  void AddSpecularSmoothnessCost(ceres::Problem& problem,
    ceres::LossFunction* loss_function);
  void AddSpecularMagnitudeCost(ceres::Problem& problem,
    ceres::LossFunction* loss_function);
  void AddTemporalSpecularCost(ceres::Problem& problem,
    ceres::LossFunction* loss_function);

  void AddVariableMask(ceres::Problem& problem, baType BA);
  void AddConstantMask(ceres::Problem& problem, baType BA);

  void KnownCorrespondencesICP(PangaeaMeshData& templateMesh,
                              PangaeaMeshData& currentMesh,
                              double camPose[6]);

  void GetDeformation(PangaeaMeshData& templateMesh,
                      PangaeaMeshData& currentMesh,
                      double camPose[6],
                      MeshDeformation& meshTrans,
                      MeshDeformation& meshRot);

  //
  void EnergySetup(ceres::Problem& problem);
  void EnergyMinimization(ceres::Problem& problem);
  void RegTermsSetup(ceres::Problem& problem, WeightPara& weightParaLevel);

  void EnergyMinimizationGT(ceres::Problem& problem);

  void AddGroundTruthConstantMask(ceres::Problem& problem);
  void AddGroundTruthVariableMask(ceres::Problem& problem);

  double ComputeRMSError(PangaeaMeshData& results, PangaeaMeshData& resultsGT);
  void CheckNaN();

  //
  bool SaveData();
  bool SaveMeshToFile(TrackerOutputInfo& outputInfo);
  bool SaveMeshPyramid();
  void SaveThread(TrackerOutputInfo** pOutputInfoRendering);

  void UpdateResults();
  void UpdateResultsLevel(int level);
  void PropagateMesh();
  void PropagateMeshCoarseToFine(int coarse_level, int fine_level);

  // attach features to the meshes
  void AttachFeaturesToMeshPyramid();
  void AttachFeatureToMesh(PangaeaMeshData* pMesh,
                           FeatureLevel* pFeatureLevel,
                           CameraInfo* pCamera,
                           vector<bool>& visibilityMask,
                           PangaeaMeshData* pOutputMesh);

  // helper functions for data term
  void AddCostImageProjection(ceres::Problem& problem,
                              ceres::LossFunction* loss_function,
                              dataTermErrorType errorType,
                              PangaeaMeshData& templateMesh,
                              MeshDeformation& meshTrans,
                              vector<bool>& visibilityMask,
                              CameraInfo* pCamera,
                              Level* pFrame);

  void AddCostImageProjection(ceres::Problem& problem,
	  ceres::LossFunction* loss_function,
	  dataTermErrorType errorType,
	  PangaeaMeshData& templateMesh,
	  MeshDeformation& meshTrans,
	  vector<bool>& visibilityMask,
	  CameraInfo* pCamera,
	  Level* pFrame,
	  MeshDeformation& local_ligthing);

  void AddCostImageProjectionPatch(ceres::Problem& problem,
                                   ceres::LossFunction* loss_function,
                                   dataTermErrorType errorType,
                                   PangaeaMeshData& templateMesh,
                                   MeshDeformation& meshTrans,
                                   vector<bool>& visibilityMask,
                                   MeshNeighbors& patchNeighbors,
                                   MeshNeighbors& patchRadii,
                                   MeshWeights& patchWeights,
                                   CameraInfo* pCamera,
                                   Level* pFrame);

  void AddCostImageProjectionCoarse(ceres::Problem& problem,
                                    ceres::LossFunction* loss_function,
                                    dataTermErrorType errorType,
                                    PangaeaMeshData& templateMesh,
                                    vector<bool>& visibilityMask,
                                    PangaeaMeshData& templateNeighborMesh,
                                    MeshDeformation& neighborMeshTrans,
                                    MeshDeformation& neighborMeshRot,
                                    MeshNeighbors& neighbors,
                                    MeshWeights& weights,
                                    CameraInfo* pCamera,
                                    Level* pFrame);

  void AddCostImageProjectionPatchCoarse(ceres::Problem& problem,
                                         ceres::LossFunction* loss_function,
                                         dataTermErrorType& errorType,
                                         PangaeaMeshData& templateMesh,
                                         vector<bool>& visibilityMask,
                                         MeshNeighbors& patchNeighbors,
                                         MeshNeighbors& patchRadii,
                                         MeshWeights& patchWeights,
                                         PangaeaMeshData& templateNeighborMesh,
                                         MeshDeformation& neighborMeshTrans,
                                         MeshDeformation& neighborMeshRot,
                                         MeshNeighbors& neighbors,
                                         MeshWeights& weights,
                                         CameraInfo* pCamera,
                                         Level* pFrame);

  dataTermErrorType getPEType();

  void setMeshIntensityPyramid(PangaeaMeshPyramid &_templateIntensityPyramid);

private:

  // Siggraph14 or DynamicFusion
  OptimizationStrategy* pStrategy;
  int currLevel;  // current optimization level

  int startFrameNo;
  int currentFrameNo;

  int m_nWidth;
  int m_nHeight;
  int m_nMeshLevels;

  baType BAType;
  dataTermErrorType PEType;

  double prevCamPose[6];
  double camPose[6];
  double KK[3][3];

  // Spherical Harmonic Coefficients for representing illumination
  //vector<double> sh_coeff;
  // Spherical Harmonic order
  //int sh_order;

  //
  CameraInfo camInfo;
  bool trackerInitialized;

  // containes the neighbor & weight information between pairs of meshes
  MeshPropagation meshPropagation;

  // ImagePyramid
  ImagePyramid* pImagePyramid;

  bool dataInBuffer;

  // FeaturePyramid
  FeaturePyramid* pFeaturePyramid;

  // visibilityMask Pyramid
  vector<vector<bool> > visibilityMaskPyramid;

  // tracker output Pyramid
  vector< TrackerOutputInfo > outputInfoPyramid;
  // keep the mesh just after propagation
  // should think a way to save memory
  vector< TrackerOutputInfo > outputPropPyramid;

  // transformation Pyramid
  PangaeaMeshPyramid templateMeshPyramid;
  vector< MeshDeformation > meshTransPyramid;
  vector< MeshDeformation > meshRotPyramid;
  vector< MeshDeformation > prevMeshTransPyramid;
  vector< MeshDeformation > prevMeshRotPyramid;

  PangaeaMeshPyramid templateIntensityPyramid;

  vector< MeshDeformation > templateAlbedoPyramid;

  vector<MeshNeighborsNano> fineToCoarseNeighbours;
  vector<MeshWeights> fineToCoarseWeights;

  uchar*** qx_image;
  uchar*** qx_diffuse_image;

  // what about if we use dual quarternion representation?
  // In that case, we will need a dual quarternion
  // field transformation

  // ceres output
  std::ofstream ceresOutput;
  std::ofstream energyOutput;
  std::ofstream errorOutput;
  std::ofstream energyOutputForR;
  std::ofstream errorOutputForR;

  vector<std::string> costNames;

  boost::thread* preProcessingThread;
  boost::thread* savingThread;

  boost::thread* featureThread;

  // attach features to the mesh
  boost::thread* attachFeatureThread;

  //problem wrapper
  ProblemWrapper problemWrapper;
  bool useProblemWrapper;  // for debug

  // for evaulation on ground truth
  PangaeaMeshPyramid templateMeshPyramidGT;
  PangaeaMeshPyramid currentMeshPyramidGT;
  vector< MeshDeformation > meshTransPyramidGT;
  vector< MeshDeformation > meshRotPyramidGT;
  vector< MeshDeformation > prevMeshTransPyramidGT;
  vector< MeshDeformation > prevMeshRotPyramidGT;

  // ground truth visibilityMask Pyramid
  vector<vector<bool > > visibilityMaskPyramidGT;

  ProblemWrapper problemWrapperGT;

  double prevCamPoseGT[6];
  double camPoseGT[6];

  // set this to true when doing optimization on ground truth data
  bool modeGT;

  // recording the average error over the whole sequence
  double meanError;
  std::ofstream scoresOutput;

  // Estimates sh coeff, albedo, and local lighting maps
  void updateIntrinsics(unsigned char* pColorImageRGB);

  void initProjectedValues(const vector<vector<CoordinateType> > &meshProj, 
	  const vector<bool> &visibility, const InternalColorImageType &colorImage,
	  const InternalIntensityImageType &brightnessImage,
	  const InternalIntensityImageType &specularImage, 
	  vector< vector<double> > &intensities, 
	  vector<double> &brightness,
	  vector< vector<double> > &local_lightings);

  void projectValues( const vector<vector<CoordinateType> > &meshProj,
  const vector<bool> &visibility, const InternalColorImageType &colorImage,
  vector< vector<double> > &intensities);

  void estimateSHCoeff(const PangaeaMeshData &mesh, 
	  const vector<bool> &visibility, const vector<vector<double>> &intensities,
	  vector<vector<double>> &albedos, const vector<double> &specular_weights,
	  const int sh_order, vector<double> &sh_coeff);

  void estimateSHCoeff( const PangaeaMeshData &mesh, 
    const vector<bool> &visibility, const vector<vector<double>> &intensities,
    vector<vector<double>> &albedos, vector<vector<double>> &local_lightings,
    const int sh_order, vector<double> &sh_coeff);

  void updateShading(const PangaeaMeshData &mesh,
	  const vector<double> &sh_coeff, const int sh_order,
	  vector<double> &shadings);

  void estimateAlbedo(const PangaeaMeshData &mesh,
	  const vector<bool> &visibility,
	  const MeshDeformation &intensities,
	  const vector<double> &shadings,
	  const MeshDeformation &local_lightings,
	  const vector<double> &specular_weights, 
	  const MeshDeformation &template_albedos,
	  MeshDeformation &albedos);

  double computeDiffWeight(
	  const MeshDeformation &local_lightings, const MeshDeformation &colors,
	  const unsigned int v_idx1, const unsigned int v_idx2);

  void estimateLocalLighting(const PangaeaMeshData &mesh,
	  const vector<bool> &visibility,
	  const MeshDeformation &intensities,
	  const MeshDeformation &albedos,
	  const vector<double> &shadings,
	  MeshDeformation &local_lightings);

  void estimateSHCoeffLocalLighting(const PangaeaMeshData &mesh,
      const vector<bool> &visibility,
      const MeshDeformation &intensities,
      MeshDeformation &albedos,
      const int sh_order, vector<double> &sh_coeff,
      MeshDeformation &local_lightings);

  void initNeighboursWeightsFineToCoarse();

  void propagateAlbedoLocalLightingFineToCoarse();

  void estimateDiffuse(const cv::Mat color_image, cv::Mat &diffuse_image);

};

#endif
