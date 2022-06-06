#include "Model/Model.h"

#include "assimp/Importer.hpp"	// C++ importer interface
#include "assimp/scene.h"		    // output data structure
#include "assimp/postprocess.h"	// post processing flags

namespace library
{
    std::vector<UINT> tmp;
    XMMATRIX ConvertMatrix(_In_ const aiMatrix4x4& matrix)
    {
        return XMMATRIX(
            matrix.a1,
            matrix.b1,
            matrix.c1,
            matrix.d1,
            matrix.a2,
            matrix.b2,
            matrix.c2,
            matrix.d2,
            matrix.a3,
            matrix.b3,
            matrix.c3,
            matrix.d3,
            matrix.a4,
            matrix.b4,
            matrix.c4,
            matrix.d4
        );
    }

    XMFLOAT3 ConvertVector3dToFloat3(_In_ const aiVector3D& vector)
    {
        return XMFLOAT3(vector.x, vector.y, vector.z);
    }

    XMVECTOR ConvertQuaternionToVector(_In_ const aiQuaternion& quaternion)
    {
        XMFLOAT4 float4 = XMFLOAT4(quaternion.x, quaternion.y, quaternion.z, quaternion.w);
        return XMLoadFloat4(&float4);
    }

    std::unique_ptr<Assimp::Importer> Model::sm_pImporter = std::make_unique<Assimp::Importer>();

    Model::Model(_In_ const std::filesystem::path& filePath) :
        Renderable(XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f))
        , m_filePath(filePath)
        , m_animationBuffer(nullptr)
        , m_skinningConstantBuffer(nullptr)
        , m_aVertices(std::vector<SimpleVertex>())
        , m_aAnimationData(std::vector<AnimationData>())
        , m_aIndices(std::vector<WORD>())
        , m_aBoneData(std::vector<VertexBoneData>())
        , m_aBoneInfo(std::vector<BoneInfo>())
        , m_aTransforms(std::vector<XMMATRIX>())
        , m_boneNameToIndexMap(std::unordered_map<std::string, UINT>())
        , m_pScene(nullptr)
        , m_timeSinceLoaded(0)
        , m_globalInverseTransform(XMMatrixIdentity())


    {};

    HRESULT Model::Initialize(_In_ ID3D11Device* pDevice, _In_ ID3D11DeviceContext* pImmediateContext)
    {
        HRESULT hr = S_OK;

        m_pScene = sm_pImporter->ReadFile(
            m_filePath.string().c_str(),
            ASSIMP_LOAD_FLAGS
        );


        m_pScene = sm_pImporter->GetOrphanedScene();
        if (m_pScene)
        {

            m_globalInverseTransform =
                XMMatrixTranspose(ConvertMatrix(m_pScene->mRootNode->mTransformation));

            XMMatrixInverse(nullptr, m_globalInverseTransform);

            hr = initFromScene(pDevice, pImmediateContext, m_pScene, m_filePath);
            if (FAILED(hr)) return (hr);


        }
        else
        {
            hr = E_FAIL;
            OutputDebugString(L"Error parsing ");
            OutputDebugString(m_filePath.c_str());
            OutputDebugString(L": ");
            OutputDebugStringA(sm_pImporter->GetErrorString());
            OutputDebugString(L"\n");
        }
        //Create the Vertex buffer
        D3D11_BUFFER_DESC bufferDesc = {
        .ByteWidth = (sizeof(AnimationData) * GetNumVertices()),
        .Usage = D3D11_USAGE_DEFAULT,
        .BindFlags = D3D11_BIND_VERTEX_BUFFER,
        .CPUAccessFlags = 0,
        .MiscFlags = 0,
        .StructureByteStride = 0
        };

        D3D11_SUBRESOURCE_DATA subData = {
        .pSysMem = m_aAnimationData.data(),
        .SysMemPitch = 0,
        .SysMemSlicePitch = 0,
        };

        hr = pDevice->CreateBuffer(&bufferDesc, &subData, m_animationBuffer.GetAddressOf());
        if (FAILED(hr)) return (hr);

        D3D11_BUFFER_DESC bd;
        bd.ByteWidth = sizeof(CBSkinning);
        bd.Usage = D3D11_USAGE_DEFAULT;
        bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        bd.CPUAccessFlags = 0;
        bd.MiscFlags = 0;
        bd.StructureByteStride = 0;

        hr = pDevice->CreateBuffer(
            &bd,
            nullptr,
            m_skinningConstantBuffer.GetAddressOf()
        );
        if (FAILED(hr)) return hr;
        return hr;
    }
    void Model::Update(_In_ FLOAT deltaTime)
    {
        UNREFERENCED_PARAMETER(deltaTime);

        m_timeSinceLoaded += deltaTime;

        if (m_pScene->HasAnimations() == true) {
            if (m_pScene->mRootNode != nullptr) {
                if (m_pScene->mAnimations != nullptr) {
                }

            }
        }
    }

    ComPtr<ID3D11Buffer>& Model::GetAnimationBuffer()
    {
        return m_animationBuffer;
    }

    ComPtr<ID3D11Buffer>& Model::GetSkinningConstantBuffer()
    {
        return m_skinningConstantBuffer;
    }

    UINT Model::GetNumVertices() const
    {
        return static_cast<UINT>(m_aVertices.size());
    }

    UINT Model::GetNumIndices() const
    {
        return static_cast<UINT>(m_aIndices.size());
    }

    std::vector<XMMATRIX>& Model::GetBoneTransforms()
    {
        return m_aTransforms;
    }

    const std::unordered_map<std::string, UINT>& Model::GetBoneNameToIndexMap() const
    {
        return m_boneNameToIndexMap;
    }

    void Model::countVerticesAndIndices(_Inout_ UINT& uOutNumVertices, _Inout_ UINT& uOutNumIndices, _In_ const aiScene* pScene) {



        UINT uNumVertices = 0u;
        UINT uNumIndices = 0u;
        for (UINT i = 0u; i < pScene->mNumMeshes; ++i)
        {
            m_aMeshes[i].uMaterialIndex =
                pScene->mMeshes[i]->mMaterialIndex;
            m_aMeshes[i].uNumIndices = pScene->mMeshes[i]->mNumFaces * 3u;
            m_aMeshes[i].uBaseVertex = uNumVertices;
            m_aMeshes[i].uBaseIndex = uNumIndices;

            uNumVertices += pScene->mMeshes[i]->mNumVertices;
            uNumIndices += m_aMeshes[i].uNumIndices;
        };
        uOutNumIndices = uNumIndices;
        uOutNumVertices = uNumVertices;
    }
    const aiNodeAnim* Model::findNodeAnimOrNull(_In_ const aiAnimation* pAnimation, _In_ PCSTR pszNodeName)
    {
        for (UINT i = 0u; i < pAnimation->mNumChannels; ++i)
        {
            const aiNodeAnim* pNodeAnim = pAnimation->mChannels[i];

            if (strncmp(pNodeAnim->mNodeName.data, pszNodeName, pNodeAnim->mNodeName.length) == 0)
            {
                return pNodeAnim;
            }
        }

        return nullptr;
    }

    UINT Model::findPosition(_In_ FLOAT animationTimeTicks, _In_ const aiNodeAnim* pNodeAnim)
    {
        assert(pNodeAnim->mNumPositionKeys > 0);

        for (UINT i = 0; i < pNodeAnim->mNumPositionKeys - 1; ++i)
        {
            FLOAT t = static_cast<FLOAT>(pNodeAnim->mPositionKeys[i + 1].mTime);

            if (animationTimeTicks < t)
            {
                return i;
            }
        }

        return 0u;
    }

    UINT Model::findRotation(_In_ FLOAT animationTimeTicks, _In_ const aiNodeAnim* pNodeAnim)
    {
        assert(pNodeAnim->mNumRotationKeys > 0);

        for (UINT i = 0u; i < pNodeAnim->mNumRotationKeys - 1; ++i)
        {
            FLOAT t = static_cast<FLOAT>(pNodeAnim->mRotationKeys[i + 1].mTime);

            if (animationTimeTicks < t)
            {
                return i;
            }
        }

        return 0u;
    }

    UINT Model::findScaling(_In_ FLOAT animationTimeTicks, _In_ const aiNodeAnim* pNodeAnim)
    {
        assert(pNodeAnim->mNumScalingKeys > 0);

        for (UINT i = 0u; i < pNodeAnim->mNumScalingKeys - 1; ++i)
        {
            FLOAT t = static_cast<FLOAT>(pNodeAnim->mScalingKeys[i + 1].mTime);

            if (animationTimeTicks < t)
            {
                return i;
            }
        }

        return 0u;
    }


    UINT Model::getBoneId(_In_ const aiBone* pBone)
    {
        UINT uBoneIndex = 0u;
        PCSTR pszBoneName = pBone->mName.C_Str();

        if (!m_boneNameToIndexMap.contains(pszBoneName))
        {
            uBoneIndex = static_cast<UINT>(m_boneNameToIndexMap.size());
            m_boneNameToIndexMap[pszBoneName] = uBoneIndex;
        }
        else
        {
            uBoneIndex = m_boneNameToIndexMap[pszBoneName];
        }

        return uBoneIndex;
    }

    const SimpleVertex* Model::getVertices() const
    {
        return m_aVertices.data();
    }

    const WORD* Model::getIndices() const
    {
        return m_aIndices.data();
    }

    void Model::initAllMeshes(_In_ const aiScene* pScene)
    {
        for (UINT i = 0u; i < m_aMeshes.size(); ++i)
        {
            const aiMesh* pMesh = pScene->mMeshes[i];
            initSingleMesh(i, pMesh);
        }
    }

    HRESULT Model::loadNormalTexture(_In_ ID3D11Device* pDevice, _In_ ID3D11DeviceContext* pImmediateContext, _In_ const std::filesystem::path& parentDirectory, _In_ const aiMaterial* pMaterial, _In_ UINT uIndex)
    {
        HRESULT hr = S_OK;
        m_aMaterials[uIndex]->pNormal = nullptr;

        if (pMaterial->GetTextureCount(aiTextureType_HEIGHT) > 0)
        {
            aiString aiPath;

            if (pMaterial->GetTexture(aiTextureType_HEIGHT, 0u, &aiPath, nullptr, nullptr, nullptr, nullptr, nullptr) == AI_SUCCESS)
            {
                std::string szPath(aiPath.data);

                if (szPath.substr(0ull, 2ull) == ".\\")
                {
                    szPath = szPath.substr(2ull, szPath.size() - 2ull);
                }

                std::filesystem::path fullPath = parentDirectory / szPath;

                m_aMaterials[uIndex]->pNormal = std::make_shared<Texture>(fullPath);
                m_bHasNormalMap = true;

                if (FAILED(hr))
                {
                    OutputDebugString(L"Error loading normal texture \"");
                    OutputDebugString(fullPath.c_str());
                    OutputDebugString(L"\"\n");

                    return hr;
                }

                OutputDebugString(L"Loaded normal texture \"");
                OutputDebugString(fullPath.c_str());
                OutputDebugString(L"\"\n");
            }
        }

        return hr;
    }

    HRESULT Model::initFromScene(
        _In_ ID3D11Device* pDevice,
        _In_ ID3D11DeviceContext* pImmediateContext,
        _In_ const aiScene* pScene,
        _In_ const std::filesystem::path& filePath
    ) {
        m_aMeshes.resize(pScene->mNumMeshes);
        unsigned int NumVertices = 0u;
        unsigned int NumIndices = 0u;
        countVerticesAndIndices(NumVertices, NumIndices, m_pScene);
        reserveSpace(NumVertices, NumIndices);
        initAllMeshes(m_pScene);
        initMaterials(pDevice, pImmediateContext, m_pScene, filePath);

        m_aAnimationData.resize(GetNumVertices());

        for (UINT idxVertice = 0u; idxVertice < GetNumVertices(); idxVertice++) {
            AnimationData aData;
            aData.aBoneIndices = XMUINT4(m_aBoneData[idxVertice].aBoneIds[0],
                m_aBoneData[idxVertice].aBoneIds[1],
                m_aBoneData[idxVertice].aBoneIds[2],
                m_aBoneData[idxVertice].aBoneIds[3]
            );
            aData.aBoneWeights = XMFLOAT4(m_aBoneData[idxVertice].aWeights[0],
                m_aBoneData[idxVertice].aWeights[1],
                m_aBoneData[idxVertice].aWeights[2],
                m_aBoneData[idxVertice].aWeights[3]);


            m_aAnimationData[idxVertice] = aData;

        };


        initialize(pDevice, pImmediateContext);

        return S_OK;
    }


    HRESULT Model::initMaterials(
        _In_ ID3D11Device* pDevice,
        _In_ ID3D11DeviceContext* pImmediateContext,
        _In_ const aiScene* pScene,
        _In_ const std::filesystem::path& filePath
    )
    {
        HRESULT hr = S_OK;

        std::filesystem::path parentDirectory = filePath.parent_path();

        for (UINT i = 0u; i < pScene->mNumMaterials; ++i)
        {
            const aiMaterial* pMaterial = pScene->mMaterials[i];

            std::string szName = filePath.string() + std::to_string(i);
            std::wstring pwszName(szName.length(), L' ');
            std::copy(szName.begin(), szName.end(), pwszName.begin());
            m_aMaterials.push_back(std::make_shared<Material>(pwszName));

            loadTextures(pDevice, pImmediateContext, parentDirectory, pMaterial, i);
        }

        return hr;
    }

    HRESULT Model::loadDiffuseTexture(
        _In_ ID3D11Device* pDevice,
        _In_ ID3D11DeviceContext* pImmediateContext,
        _In_ const std::filesystem::path& parentDirectory,
        _In_ const aiMaterial* pMaterial,
        _In_ UINT uIndex
    )
    {
        HRESULT hr = S_OK;
        m_aMaterials[uIndex]->pDiffuse = nullptr;

        if (pMaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0)
        {
            aiString aiPath;

            if (pMaterial->GetTexture(aiTextureType_DIFFUSE, 0u, &aiPath, nullptr, nullptr, nullptr, nullptr, nullptr) == AI_SUCCESS)
            {
                std::string szPath(aiPath.data);

                if (szPath.substr(0ull, 2ull) == ".\\")
                {
                    szPath = szPath.substr(2ull, szPath.size() - 2ull);
                }

                std::filesystem::path fullPath = parentDirectory / szPath;

                m_aMaterials[uIndex]->pDiffuse = std::make_shared<Texture>(fullPath);

                hr = m_aMaterials[uIndex]->pDiffuse->Initialize(pDevice, pImmediateContext);
                if (FAILED(hr))
                {
                    OutputDebugString(L"Error loading diffuse texture \"");
                    OutputDebugString(fullPath.c_str());
                    OutputDebugString(L"\"\n");

                    return hr;
                }

                OutputDebugString(L"Loaded diffuse texture \"");
                OutputDebugString(fullPath.c_str());
                OutputDebugString(L"\"\n");
            }
        }

        return hr;
    }


    HRESULT Model::loadSpecularTexture(
        _In_ ID3D11Device* pDevice,
        _In_ ID3D11DeviceContext* pImmediateContext,
        _In_ const std::filesystem::path& parentDirectory,
        _In_ const aiMaterial* pMaterial,
        _In_ UINT uIndex
    )
    {
        HRESULT hr = S_OK;
        m_aMaterials[uIndex]->pSpecularExponent = nullptr;

        if (pMaterial->GetTextureCount(aiTextureType_SHININESS) > 0)
        {
            aiString aiPath;

            if (pMaterial->GetTexture(aiTextureType_SHININESS, 0u, &aiPath, nullptr, nullptr, nullptr, nullptr, nullptr) == AI_SUCCESS)
            {
                std::string szPath(aiPath.data);

                if (szPath.substr(0ull, 2ull) == ".\\")
                {
                    szPath = szPath.substr(2ull, szPath.size() - 2ull);
                }

                std::filesystem::path fullPath = parentDirectory / szPath;

                m_aMaterials[uIndex]->pSpecularExponent = std::make_shared<Texture>(fullPath);

                hr = m_aMaterials[uIndex]->pSpecularExponent->Initialize(pDevice, pImmediateContext);
                if (FAILED(hr))
                {
                    OutputDebugString(L"Error loading specular texture \"");
                    OutputDebugString(fullPath.c_str());
                    OutputDebugString(L"\"\n");

                    return hr;
                }

                OutputDebugString(L"Loaded specular texture \"");
                OutputDebugString(fullPath.c_str());
                OutputDebugString(L"\"\n");
            }
        }

        return hr;
    }



    HRESULT Model::loadTextures(
        _In_ ID3D11Device* pDevice,
        _In_ ID3D11DeviceContext* pImmediateContext,
        _In_ const std::filesystem::path& parentDirectory,
        _In_ const aiMaterial* pMaterial,
        _In_ UINT uIndex
    )
    {
        HRESULT hr = loadDiffuseTexture(pDevice, pImmediateContext, parentDirectory, pMaterial, uIndex);
        if (FAILED(hr))
        {
            return hr;
        }

        hr = loadSpecularTexture(pDevice, pImmediateContext, parentDirectory, pMaterial, uIndex);
        if (FAILED(hr))
        {
            return hr;
        }
        hr = loadNormalTexture(pDevice, pImmediateContext, parentDirectory, pMaterial, uIndex);
        if (FAILED(hr))
        {
            return hr;
        }
        return hr;
    }


    void Model::reserveSpace(_In_ UINT uNumVertices, _In_ UINT uNumIndices) {
        m_aVertices.reserve(uNumVertices);
        m_aIndices.reserve(uNumIndices);
        m_aBoneData.resize(uNumVertices);
    }

    void Model::initMeshSingleBone(_In_ UINT uMeshIndex, _In_ const aiBone* pBone)
    {
        UINT uBoneId = getBoneId(pBone);
        if (uBoneId == m_aBoneInfo.size())
        {
            BoneInfo boneInfo(ConvertMatrix(pBone->mOffsetMatrix));
            m_aBoneInfo.push_back(boneInfo);
        }

        for (UINT i = 0u; i < pBone->mNumWeights; i++)
        {
            const aiVertexWeight& vertexWeight = pBone->mWeights[i];
            UINT uGlobalVertexId = m_aMeshes[uMeshIndex].uBaseVertex + vertexWeight.mVertexId;
            m_aBoneData[uGlobalVertexId].AddBoneData(uBoneId, vertexWeight.mWeight);
        }
    }

    void Model::initMeshBones(_In_ UINT uMeshIndex, _In_ const aiMesh* pMesh) {
        UINT numBones = 0u;
        for (UINT i = 0; i < pMesh->mNumBones; i++) {
            initMeshSingleBone(uMeshIndex, pMesh->mBones[i]);
        }
    }
    void Model::initSingleMesh(_In_ UINT uMeshIndex, _In_ const aiMesh* pMesh) {
        for (int i = 0; i < pMesh->mNumVertices; i++) {
            const aiVector3D zero3d(0.0f, 0.0f, 0.0f);
            const aiVector3D& position = pMesh->mVertices[i];
            const aiVector3D& normal = pMesh->mNormals[i];
            const aiVector3D& texCoord = pMesh->HasTextureCoords(0u) ?
                pMesh->mTextureCoords[0][i] : zero3d;
            const aiVector3D& tangent = pMesh->HasTangentsAndBitangents() ?
                pMesh->mTangents[i] : zero3d;
            const aiVector3D& bitangent = pMesh->HasTangentsAndBitangents() ?
                pMesh->mBitangents[i] : zero3d;

            NormalData normalData =
            {
                .Tangent = XMFLOAT3(tangent.x,tangent.y,tangent.z),
                .Bitangent = XMFLOAT3(bitangent.x,bitangent.y,bitangent.z)
            };
            SimpleVertex vertex =
            {
            .Position = XMFLOAT3(position.x, position.y, position.z),
            .TexCoord = XMFLOAT2(texCoord.x, texCoord.y),
            .Normal = XMFLOAT3(normal.x, normal.y, normal.z)
            };

            m_aVertices.push_back(vertex);
            m_aNormalData.push_back(normalData);
        }

        for (int i = 0; i < pMesh->mNumFaces; i++) {
            const aiFace& face = pMesh->mFaces[i];
            assert(face.mNumIndices == 3u);
            WORD aIndices[3] =
            {
            static_cast<WORD>(face.mIndices[0]),
            static_cast<WORD>(face.mIndices[1]),
            static_cast<WORD>(face.mIndices[2]),
            };

            m_aIndices.push_back(aIndices[0]);
            m_aIndices.push_back(aIndices[1]);
            m_aIndices.push_back(aIndices[2]);

        }
        initMeshBones(uMeshIndex, pMesh);
        


    }

    void Model::interpolatePosition(_Inout_ XMFLOAT3& outTranslate, _In_ FLOAT animationTimeTicks, _In_ const aiNodeAnim* pNodeAnim)
    {
        if (pNodeAnim->mNumPositionKeys == 1)
        {
            outTranslate = ConvertVector3dToFloat3(pNodeAnim->mPositionKeys[0].mValue);
            return;
        }

        UINT uPositionIndex = findPosition(animationTimeTicks, pNodeAnim);
        UINT uNextPositionIndex = uPositionIndex + 1u;
        assert(uNextPositionIndex < pNodeAnim->mNumPositionKeys);

        FLOAT t1 = static_cast<FLOAT>(pNodeAnim->mPositionKeys[uPositionIndex].mTime);
        FLOAT t2 = static_cast<FLOAT>(pNodeAnim->mPositionKeys[uNextPositionIndex].mTime);
        FLOAT deltaTime = t2 - t1;
        FLOAT factor = (animationTimeTicks - t1) / deltaTime;
        assert(factor >= 0.0f && factor <= 1.0f);
        const aiVector3D& start = pNodeAnim->mPositionKeys[uPositionIndex].mValue;
        const aiVector3D& end = pNodeAnim->mPositionKeys[uNextPositionIndex].mValue;
        aiVector3D delta = end - start;
        outTranslate = ConvertVector3dToFloat3(start + factor * delta);
    }
    void Model::interpolateRotation(_Inout_ XMVECTOR& outQuaternion, _In_ FLOAT animationTimeTicks, _In_ const aiNodeAnim* pNodeAnim) {
        aiQuaternion ret;
        if (pNodeAnim->mNumRotationKeys == 1)
        {
            ret = pNodeAnim->mRotationKeys[0].mValue;
            outQuaternion = ConvertQuaternionToVector(ret);
            return;
        }

        UINT keyIndex = findRotation(animationTimeTicks, pNodeAnim);
        UINT nextKeyIndex = keyIndex + 1;

        float deltaTime = pNodeAnim->mRotationKeys[nextKeyIndex].mTime - pNodeAnim->mRotationKeys[keyIndex].mTime;
        float factor = (animationTimeTicks - (float)pNodeAnim->mRotationKeys[keyIndex].mTime) / deltaTime;

        assert(factor >= 0.0f && factor <= 1.0f);

        const aiQuaternion& startValue = pNodeAnim->mRotationKeys[keyIndex].mValue;
        const aiQuaternion& endValue = pNodeAnim->mRotationKeys[nextKeyIndex].mValue;
        aiQuaternion::Interpolate(ret, startValue, endValue, factor);
        outQuaternion = ConvertQuaternionToVector(ret);
    }
    void Model::interpolateScaling(_Inout_ XMFLOAT3& outScale, _In_ FLOAT animationTimeTicks, _In_ const aiNodeAnim* pNodeAnim) {
        if (pNodeAnim->mNumScalingKeys == 1)
        {
            outScale = ConvertVector3dToFloat3(pNodeAnim->mScalingKeys[0].mValue);
            return;
        }

        UINT uScalingIndex = findScaling(animationTimeTicks, pNodeAnim);
        UINT uNextScalingIndex = uScalingIndex + 1u;
        assert(uNextScalingIndex < pNodeAnim->mNumScalingKeys);

        FLOAT t1 = static_cast<FLOAT>(pNodeAnim->mScalingKeys[uScalingIndex].mTime);
        FLOAT t2 = static_cast<FLOAT>(pNodeAnim->mScalingKeys[uNextScalingIndex].mTime);
        FLOAT deltaTime = t2 - t1;
        FLOAT factor = (animationTimeTicks - t1) / deltaTime;
        assert(factor >= 0.0f && factor <= 1.0f);
        const aiVector3D& start = pNodeAnim->mScalingKeys[uScalingIndex].mValue;
        const aiVector3D& end = pNodeAnim->mScalingKeys[uNextScalingIndex].mValue;
        aiVector3D delta = end - start;
        outScale = ConvertVector3dToFloat3(start + factor * delta);
    }


    void Model::readNodeHierarchy(_In_ FLOAT animationTimeTicks, _In_ const aiNode* pNode, _In_ const XMMATRIX& parentTransform) {
        const aiAnimation* animation = m_pScene->mAnimations[0];
        XMMATRIX nodeTransform = ConvertMatrix(pNode->mTransformation);

        const aiNodeAnim* nodeAnim = findNodeAnimOrNull(animation, pNode->mName.data);
        if (nodeAnim) {
            //SCALING
            XMFLOAT3 scaling = XMFLOAT3(0.0f, 0.0f, 0.0f);

            interpolateScaling(scaling, animationTimeTicks, nodeAnim);

            XMMATRIX scalingM = XMMatrixScaling(scaling.x, scaling.y, scaling.z);
            //ROTATION
            XMVECTOR rotating;
            interpolateRotation(rotating, animationTimeTicks, nodeAnim);
            XMMATRIX rotationM = XMMatrixRotationQuaternion(rotating);

            //TRANSLATION
            XMFLOAT3 translation;
            interpolatePosition(translation, animationTimeTicks, nodeAnim);
            XMMATRIX translationM = XMMatrixTranslation(translation.x, translation.y, translation.z);

            nodeTransform = scalingM * rotationM * translationM;
        }
        ///
        XMMATRIX globalTransformation = nodeTransform * parentTransform;
        ///

        if (m_boneNameToIndexMap.find(pNode->mName.data) != m_boneNameToIndexMap.end()) {
            UINT boneIndex = m_boneNameToIndexMap[pNode->mName.data];
            m_aBoneInfo[boneIndex].FinalTransformation =
                m_aBoneInfo[boneIndex].OffsetMatrix *
                globalTransformation *
                m_globalInverseTransform;
        }

        for (UINT i = 0; i < pNode->mNumChildren; i++) {
            readNodeHierarchy(animationTimeTicks, pNode->mChildren[i], globalTransformation);
        }
    }

}