#include <Windows.h>
#include <cmath>
#include <algorithm>

#include "Aimbot.h"
#include "include.h"
#include "Skeleton.h"

#include <random>
#include <complex>

Aimbot::Aimbot() : AutoConfigurable("Aimbot") {
    // 初始化基本参数
    registerParam("Enabled", aimParams.enabled);
    registerParam("AimType", aimParams.aimType);
    registerParam("ShieldCheck", aimParams.shieldCheck);

    // 非瞄准状态参数
    registerParam("Noscope_Smooth", aimParams.noScope.smooth);
    registerParam("Noscope_Fov", aimParams.noScope.fov);
    registerParam("Noscope_RecoilX", aimParams.noScope.recoilX);
    registerParam("Noscope_RecoilY", aimParams.noScope.recoilY);

    // 瞄准状态参数
    registerParam("Scoped_Smooth", aimParams.scoped.smooth);
    registerParam("Scoped_Fov", aimParams.scoped.fov);
    registerParam("Scoped_RecoilX", aimParams.scoped.recoilX);
    registerParam("Scoped_RecoilY", aimParams.scoped.recoilY);

    // 高级参数
    registerParam("SwitchSmoothScale", aimParams.switchSmoothScale);
    registerParam("ShootingSmoothScale", aimParams.shootingSmoothScale);
    registerParam("AutoAimBone", aimParams.autoAimBone);
    registerParam("ShowFOV", aimParams.showFOV);
    registerParam("FovThickness", aimParams.fovThickness);
    registerParam("MaxDistance", aimParams.maxDistance);
    registerParam("MinDistance", aimParams.minDistance);
    registerParam("AimBone", aimParams.aimBone);

    // 目标选择参数
    registerParam("TargetEnemies", targetSettings.targetEnemies);
    registerParam("TargetTeammates", targetSettings.targetTeammates);
    registerParam("TargetBots", targetSettings.targetBots);
    registerParam("VisibleOnly", targetSettings.visibleOnly);
    registerParam("IgnoreKnockedDown", targetSettings.ignoreKnockedDown);
    registerParam("PriorityMode", targetSettings.priorityMode);

    // 视觉设置
    registerParam("NormalColor", visualSettings.normalColor);
    registerParam("TargetColor", visualSettings.targetColor);

    // 预测设置
    registerParam("PredictionEnabled", predictionSettings.enabled);
    registerParam("PredictionScale", predictionSettings.scale);
    registerParam("ShowPredictionDebug", predictionSettings.showDebug);
    registerParam("PredictionSystem", predictionSettings.predictionSystem);
    registerParam("CacheTimeout", predictionSettings.cacheTimeout);

    // 目标特定设置
    registerParam("DefaultBoneAll", targetTypeSettings.defaultBones[0]);
    registerParam("DefaultBoneNormal", targetTypeSettings.defaultBones[1]);
    registerParam("DefaultBoneShield", targetTypeSettings.defaultBones[2]);

    registerParam("HumanizeEnabled", humanParams.enabled);
    registerParam("MicroAdjustScale", humanParams.microAdjustScale);
    registerParam("AdjusType", humanParams.type);
    registerParam("NonLinearPathStrength", humanParams.nonLinearPathStrength);
    registerParam("TimeVariationMin", humanParams.timeVariationMin);
    registerParam("TimeVariationMax", humanParams.timeVariationMax);
    registerParam("BezierSpeed", humanParams.bezierSpeed);
    registerParam("ShowLockIndicator", humanParams.showLockIndicator);
    registerParam("IndicatorColor", humanParams.indicatorColor);

    registerParam("EnablePitchAdjust", humanParams.enablePitchAdjust);
    registerParam("EnableYawAdjust", humanParams.enableYawAdjust);

    registerParam("HumanizeMode", humanParams.humanizeMode);
    registerParam("CurveOffsetMin", humanParams.curveOffsetMin);
    registerParam("CurveOffsetMax", humanParams.curveOffsetMax);

    // 初始化贝塞尔曲线参数
    humanParams.bezierProgress = 0.0f;
    humanParams.lastUpdateTime = 0.0f;
    humanParams.nextUpdateDelay = 0.0f;

    // SkyNade参数
    // SkyNade parameters removed - functionality disabled

    keys.primary = VK_LBUTTON;    // 鼠标左键
    keys.primaryController = 0;    // 鼠标左键
    // secondary and head keys removed
    // keys.trigger and keys.triggerController removed - standard trigger disabled
    keys.triggerAuto = VK_XBUTTON2;      // Mouse5 для magnet trigger
    keys.triggerAutoController = 0;      // 侧键2

    // SkyNade keys removed - functionality disabled

    // 鍵鼠/手柄綁定
    registerParam("Key_Primary", keys.primary);
    registerParam("Key_PrimaryController", keys.primaryController);
    // Secondary and head keys removed
    // Standard trigger removed - only auto trigger remains
    registerParam("Key_TriggerAuto", keys.triggerAuto);
    registerParam("Key_TriggerAutoController", keys.triggerAutoController);

    // 目标系统初始化
    targetSystem.target = nullptr;
    targetSystem.lastSwitchTime = 0;
    targetSystem.switchCooldown = 0.5f;
    targetSystem.switchSmoothDuration = 0.3f;

    isSkyNadeActive = false;
}


// 在registerHotKeys函数中添加SkyNade的键位注册
void Aimbot::registerHotKeys()
{
    std::unordered_map<std::string, int> aimbotKeys = {
        {"primary", keys.primary},
        {"primaryController", keys.primaryController},
        // Secondary and head keys removed
        // Standard trigger removed
        {"triggerAuto", keys.triggerAuto},
        {"triggerAutoController", keys.triggerAutoController}
        // Skynade keys will be removed when skynade is deleted
    };

    keyDetector->registerKeysForContext("Aimbot", aimbotKeys);
}

// 在updateKeys函数中添加SkyNade的键位更新
void Aimbot::updateKeys()
{
    auto updateKey = [this](const std::string& context, const std::string& keyName, int& key) {
        if (key == VK_ESCAPE) {
            key = 0;
        }
        keyDetector->updateKeyCode(context, keyName, key);
        };

    updateKey("Aimbot", "primary", keys.primary);
    updateKey("Aimbot", "primaryController", keys.primaryController);
    // Secondary and head keys removed
    // Standard trigger keys removed
    updateKey("Aimbot", "triggerAuto", keys.triggerAuto);
    updateKey("Aimbot", "triggerAutoController", keys.triggerAutoController);
    // SkyNade key updates removed - functionality disabled
}

// StartSkyNade function completely removed - skynade functionality disabled

void Aimbot::run() {
    while (true) {

        if (!entityManager.isLoggedIn())
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            continue;
        }

        if (!entityManager.isInGame() || !aimParams.enabled || !localPlayer || !localPlayer->IsValid()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            continue;
        }

        updateKeys();
        RunAimbot();

        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

void Aimbot::RunAimbot() {
	if (!aimParams.enabled) {
		ReleaseTarget();
		return;
	}

	if (!localPlayer || !localPlayer->IsValid()) {
		ReleaseTarget();
		return;
	}

	if (localPlayer->IsDead || localPlayer->IsKnocked || localPlayer->IsEmptyHand) {
		ReleaseTarget();
		return;
	}

	// Читаем состояние прицеливания один раз для всей функции
	bool isZooming = mem.Read<bool>(localPlayer->BasePointer + OFF_ZOOMING);

	// Check if KmBox is required but not initialized
	if (aimParams.aimType == 1 && (!kmbox || !kmbox->kmboxConfig.initialized)) {
		// Only log once every few seconds to avoid spam
		static auto lastLog = std::chrono::steady_clock::now();
		auto initTime = std::chrono::steady_clock::now();
		if (std::chrono::duration_cast<std::chrono::seconds>(initTime - lastLog).count() >= 3) {
			// std::cout << "[AimBot] Waiting for MAKCU initialization..." << std::endl;
			lastLog = initTime;
		}
		return;
	}

	// Only log run status occasionally to avoid spam
	static auto lastRunLog = std::chrono::steady_clock::now();
	auto now = std::chrono::steady_clock::now();
	if (std::chrono::duration_cast<std::chrono::milliseconds>(now - lastRunLog).count() >= 1000) {
		// std::cout << "[AimBot] Running - aimType: " << aimParams.aimType << ", enabled: " << aimParams.enabled << std::endl;
		lastRunLog = now;
	}

	// 应用时间变化，如果不应该更新瞄准则直接返回
	if (!ShouldUpdateAim()) {
		return;
	}

	bool isPrimaryKeyHeld = misc->uiParams.useController ? (keys.primaryController != 0 && isKeyHeld("primaryController")) : (keys.primary != 0 && isKeyHeld("primary"));
	// isSecondaryKeyHeld removed - secondary aim disabled
	// isTriggerKeyHeld removed - standard trigger disabled
	bool isTriggerAutoKeyHeld = misc->uiParams.useController ? (keys.triggerAutoController != 0 && isKeyHeld("triggerAutoController")) : (keys.triggerAuto != 0 && isKeyHeld("triggerAuto"));
	// isSkynadeKeyHeld removed - skynade functionality disabled

	// Trigger-only mode removed - standard trigger disabled

	// Secondary key logic restored - it should work normally for aiming

	// Debug key states - show when keys change or every 5 seconds
	static auto lastKeyDebug = std::chrono::steady_clock::now();
	static bool lastPrimary = false, lastTriggerAuto = false;

	auto currentTime = std::chrono::steady_clock::now();
	bool keysChanged = (lastPrimary != isPrimaryKeyHeld ||
					   lastTriggerAuto != isTriggerAutoKeyHeld);

	if (keysChanged || std::chrono::duration_cast<std::chrono::seconds>(currentTime - lastKeyDebug).count() >= 5) {
		// std::cout << "[AimBot] KEY STATES - Primary=" << isPrimaryKeyHeld << " (key:" << keys.primary << ")"
		// 		 << ", Secondary=" << isSecondaryKeyHeld << " (key:" << keys.secondary << ")"
		// 		 << ", Trigger=" << isTriggerKeyHeld << " (key:" << keys.trigger << ")"
		// 		 << ", TriggerAuto=" << isTriggerAutoKeyHeld << " (key:" << keys.triggerAuto << ")" << std::endl;

		// Show raw key detection
		// std::cout << "[AimBot] RAW KEYS - Primary held: " << isKeyHeld("primary")
		// 		 << ", Secondary held: " << isKeyHeld("secondary")
		// 		 << ", Trigger held: " << isKeyHeld("trigger")
		// 		 << ", TriggerAuto held: " << isKeyHeld("triggerAuto") << std::endl;

		// Simple key logging
		// if (keysChanged) {
		// 	std::cout << "[AimBot] Keys: P=" << isPrimaryKeyHeld
		// 			 << " T=" << isTriggerKeyHeld << " TA=" << isTriggerAutoKeyHeld << std::endl;
		// }

		lastKeyDebug = currentTime;
		lastPrimary = isPrimaryKeyHeld;
		// lastSecondary removed
		// lastTrigger removed - standard trigger disabled
		lastTriggerAuto = isTriggerAutoKeyHeld;
	}

	// DEBUG: Show trigger key detection status
	// if (isTriggerKeyHeld) {
	// 	std::cout << "[DEBUG] Trigger key detected as HELD" << std::endl;
	// } else {
	// 	static auto lastTriggerCheck = std::chrono::steady_clock::now();
	// 	auto now = std::chrono::steady_clock::now();
	// 	if (std::chrono::duration_cast<std::chrono::milliseconds>(now - lastTriggerCheck).count() >= 1000) {
	// 		std::cout << "[DEBUG] Trigger key NOT detected (check key binding)" << std::endl;
	// 		lastTriggerCheck = now;
	// 	}
	// }

	if (!isPrimaryKeyHeld && !isTriggerAutoKeyHeld) {
		// Log only when target was previously locked
		// if (targetSystem.target != nullptr) {
		// 	std::cout << "[AimBot] Keys released, target lost" << std::endl;
		// }
		ReleaseTarget();
		return;
	}

	// Отладка: проверим, есть ли цель перед поиском новой
	// if (targetSystem.target == nullptr) {
	// 	std::cout << "[AimBot] No current target, will try to find new one" << std::endl;
	// }

	// Key logging is now handled above with more detailed debugging
    isSkyNadeActive = false; // Skynade disabled

	// Only find targets if aim is actually active (keys are held)
	bool aimIsActive = isPrimaryKeyHeld || isTriggerAutoKeyHeld;

	if (aimIsActive && (targetSystem.target == nullptr || !IsValidTarget(targetSystem.target))) {
		ReleaseTarget();
		targetSystem.target = FindBestTarget();
		if (targetSystem.target) {
			Vec3D headPos = GetBonePosition(targetSystem.target, HitboxType::Head);
			// std::cout << "[AimBot] Found new target at distance " << CalculateDistanceFromCrosshair(headPos) << std::endl;
		}
		else {
			// Log no target found only occasionally
			// static auto lastNoTargetLog = std::chrono::steady_clock::now();
			// auto now = std::chrono::steady_clock::now();
			// if (std::chrono::duration_cast<std::chrono::seconds>(now - lastNoTargetLog).count() >= 2) {
			// 	std::cout << "[AimBot] No valid targets in range" << std::endl;
			// 	lastNoTargetLog = now;
			// }
		}
	}
	else if (!aimIsActive && targetSystem.target != nullptr) {
		// Clear target when aim becomes inactive
		ReleaseTarget();
	}
	else if (IsValidTarget(targetSystem.target)) {
        // Отладка: проверим состояние цели перед обработкой
        // static auto lastTargetCheck = std::chrono::steady_clock::now();
        // auto now = std::chrono::steady_clock::now();
        // if (std::chrono::duration_cast<std::chrono::milliseconds>(now - lastTargetCheck).count() >= 1000) {
        //     std::cout << "[AimBot] Target valid - IsAimedAt: " << (targetSystem.target->IsAimedAt ? "YES" : "NO")
        //               << ", Distance: " << targetSystem.target->DistanceToLocalPlayer << std::endl;
        //     lastTargetCheck = now;
        // }

        // SkyNade removed - functionality disabled
        // Handle magnetic trigger (aim + trigger together)
        if (isTriggerAutoKeyHeld) {
            // std::cout << "[AimBot] Starting aim + trigger (magnetic)" << std::endl;
            StartAiming(targetSystem.target);  // Aim first
            CheckAndTriggerTarget(targetSystem.target, true);  // Then trigger (magnet mode - skip AimedAt check)
        }
        // Handle regular aim (unless trigger is also held)
        else if (isPrimaryKeyHeld) {
                // Trigger-only mode removed - standard trigger disabled

                // Only log aim start occasionally to avoid spam
                // static auto lastAimStartLog = std::chrono::steady_clock::now();
                // auto now = std::chrono::steady_clock::now();
                // if (std::chrono::duration_cast<std::chrono::milliseconds>(now - lastAimStartLog).count() >= 500) {
                //     std::cout << "[AimBot] Aiming at target" << std::endl;
                //     lastAimStartLog = now;
                // }
                StartAiming(targetSystem.target);

                // Trigger while aiming removed - standard trigger disabled
            }
        // Standalone trigger functionality completely removed
        else {
            // No aiming keys held - this is normal
            return;
        }
	}
	else {
		// std::cout << "[AimBot] Releasing target" << std::endl;
		ReleaseTarget();
	}

}

void Aimbot::RenderTriggerFrame() {
    triggerbot->RenderHitboxes(targetSystem.target, isTriggering);
}

Player* Aimbot::FindBestTarget() {
    if (!localPlayer || !localPlayer->IsValid()) {
        return nullptr;
    }

    Player* bestTarget = nullptr;
    float bestScore = -999999.0f;

    // Cache FOV parameters once
    // FOV не меняется от кратности прицела - используем фиксированные настройки
    float scopedFov = aimParams.scoped.fov;
    // Проверяем прицеливание напрямую из памяти
    bool isZooming = mem.Read<bool>(localPlayer->BasePointer + OFF_ZOOMING);
    const float maxFov = isZooming ? aimParams.scoped.fov : aimParams.noScope.fov;
    const float fovScale = GetFOVScale();
    float bestFov = std::min(maxFov, maxFov * (fovScale * 1.2f));

    // Cache force head state
    bool forceHead = isKeyHeld("head");

    // Cache force bone state
    int forceBone = -1;
    if (!aimParams.autoAimBone) {
        forceBone = aimParams.aimBone;
    }

    const auto& entities = entityManager.getPlayers();

    for (auto& player : *entities) {
        // Quick validity check
        if (player == nullptr || !IsValidTarget(player)) {
            continue;
        }

        // Use cached bone position if available and valid
        Vec3D targetPos = player->BestBonePosition;
        bool needsUpdate = targetPos.empty() || 
                          (player->LastBoneUpdateTime < GetTickCount64() - 16); // Update every 16ms if needed

        if (needsUpdate) {
            bool bonePositionValid = false;

            // Try skeleton system first
            if (skeletonSystem) {
                bonePositionValid = skeletonSystem->GetBestBonePositionForAiming(player, targetPos, forceHead, forceBone);
            }

            // Fallback to original method if needed
            if (!bonePositionValid) {
                HitboxType hitboxType = forceHead ? HitboxType::Head :
                    (forceBone >= 0 ? static_cast<HitboxType>(forceBone) :
                        static_cast<HitboxType>(GetBestBone(player)));

                targetPos = GetBonePosition(player, hitboxType);
                if (targetPos.empty()) {
                    continue;
                }
            }

            // Update cache
            player->BestBonePosition = targetPos;
            player->LastBoneUpdateTime = GetTickCount64();
        }

        // Quick FOV check before expensive calculations
        float currentFOV = CalculateDistanceFromCrosshair(targetPos);
        if (currentFOV > bestFov && player != targetSystem.target) {
            continue;
        }

        // Calculate target score
        float score = CalculateTargetScore(player, bestFov);
        if (score > bestScore) {
            bestScore = score;
            bestTarget = player;
        }
    }

    return bestTarget;
}

bool Aimbot::IsValidTarget(Player* target) {
    // 检查指针有效性
    if (target == nullptr) return false;

    // 检查玩家状态
    if (!target->IsDataValid() || target->IsLocal) return false;

    // 应用目标类型过滤
    if (target->IsTeamMate && !targetSettings.targetTeammates) return false;
    if (!target->IsTeamMate && !target->IsDummy() && !targetSettings.targetEnemies) return false;
    if (target->IsDummy() && !targetSettings.targetBots) return false;

    // 可见性过
    if ((!target->IsVisible && targetSettings.visibleOnly) && !localPlayer->IsHoldingGrenade) return false;

    // Skynade check removed - functionality disabled

    // 击倒过滤
    if (targetSettings.ignoreKnockedDown && target->IsKnocked) return false;

    if (target->IsDead || target->Health <= 0) return false;
    if (!target->IsHostile && !targetSettings.targetTeammates) return false;

    const float dist = target->Distance2DToLocalPlayer;
    return (dist <= aimParams.maxDistance && dist >= aimParams.minDistance);
}

Vec3D Aimbot::AimBotHumanizeMode1(Vec3D EnemyDelta, float AccuracyMin, float AccuracyMax) {
    Vec3D n = EnemyDelta;
    Vec3D m = { 0, 0, 0 };

    // 修复1: 保留Z轴信息
    Vec3D L /* Mid Point */ = { (n.x + m.x) / 2, (n.y + m.y) / 2, (n.z + m.z) / 2 };

    // 计算目标向量在XY平面的投影长度
    float projectionLength = sqrt(n.x * n.x + n.y * n.y);

    // 防止除零错误
    if (projectionLength < 0.0001f) {
        // 如果投影几乎为零，说明目标在正上方或正下方
        // 此时使用一个任意水平方向的向量
        Vec3D horizontalDir = { 1.0f, 0, 0 };

        // 确定垂直偏移方向
        Vec3D verticalDir;
        if (std::abs(n.z) < 0.0001f) {
            // 如果z也接近零，使用向上的向量
            verticalDir = { 0, 0, 1.0f };
        }
        else {
            // 否则使用垂直于z方向的向量
            verticalDir = { 0, 1.0f, 0 };
        }

        // 生成随机偏移
        static std::random_device rd;
        static std::mt19937 gen(rd());
        static std::uniform_real_distribution<float> dis(-1.0f, 1.0f);

        float randHoriz = dis(gen);
        float randVert = dis(gen);

        // 计算偏移量
        float accuracyValue = AccuracyMin + (AccuracyMax - AccuracyMin) * 0.5f;
        float offsetScale = 0.01f;

        // 创建一个小的随机偏移向量
        Vec3D offset = {
            horizontalDir.x * randHoriz * offsetScale,
            horizontalDir.y * randHoriz * offsetScale,
            verticalDir.z * randVert * offsetScale
        };

        // 返回偏移后的向量
        return n + offset;
    }

    // 计算XY投影的单位向量
    Vec3D projectedDir = { n.x / projectionLength, n.y / projectionLength, 0 };

    // 计算垂直于XY投影的水平向量 (逆时针旋转90度)
    Vec3D perpVector = { -projectedDir.y, projectedDir.x, 0 };

    // 计算垂直于目标向量和水平垂直向量的上下方向向量
    Vec3D verticalDir;
    // 使用叉积计算垂直向量: projectedDir × perpVector
    verticalDir.x = projectedDir.y * perpVector.z - projectedDir.z * perpVector.y;
    verticalDir.y = projectedDir.z * perpVector.x - projectedDir.x * perpVector.z;
    verticalDir.z = projectedDir.x * perpVector.y - projectedDir.y * perpVector.x;

    // 归一化垂直向量
    float vertLength = sqrt(verticalDir.x * verticalDir.x +
        verticalDir.y * verticalDir.y +
        verticalDir.z * verticalDir.z);
    if (vertLength > 0.0001f) {
        verticalDir.x /= vertLength;
        verticalDir.y /= vertLength;
        verticalDir.z /= vertLength;
    }

    // 生成随机偏移
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<float> dis(-1.0f, 1.0f);

    float randHoriz = dis(gen);
    float randVert = dis(gen);

    // 修复2: 使用修改后的公式计算偏移量
    float accuracyValue = AccuracyMin + (AccuracyMax - AccuracyMin) * (0.5f + 0.5f * std::abs(randHoriz));
    float distFactor = n.length() * 0.01f;
    float offsetAmount = distFactor / accuracyValue;

    // 创建水平和垂直偏移
    Vec3D horizOffset = {
        perpVector.x * randHoriz * offsetAmount,
        perpVector.y * randHoriz * offsetAmount,
        perpVector.z * randHoriz * offsetAmount
    };

    Vec3D vertOffset = {
        verticalDir.x * randVert * offsetAmount,
        verticalDir.y * randVert * offsetAmount,
        verticalDir.z * randVert * offsetAmount
    };


    // 修复4: 返回保留z信息的最终结果
    return L + horizOffset + vertOffset;
}

Vec3D Aimbot::ApplyHumanizationToAim(Vec3D targetPos, Vec3D currentPos, int humanizeMode) {
    // 计算从当前位置到目标位置的向量
    Vec3D delta = targetPos - currentPos;

    // 应用选定的人性化模式
    switch (humanizeMode) {
    case 0: // 模式1: 曲线偏移方法
    {
        // 传递目标位置的delta和精度参数
        float accuracyMin = humanParams.curveOffsetMin;
        float accuracyMax = humanParams.curveOffsetMax;

        // 应用人性化函数
        Vec3D humanizedDelta = AimBotHumanizeMode1(delta, accuracyMin, accuracyMax);

        // 返回人性化后的目标位置
        return currentPos + humanizedDelta;
    }

    case 1: // 模式2: 原有的贝塞尔曲线方法(StartAiming中已有实现)
    default:
        // 返回原始目标位置，因为贝塞尔曲线方法在StartAiming中已实现
        return targetPos;
    }
}

float Aimbot::CalculateTargetScore(Player* player, float bestFov) {
    if (!player || !localPlayer || !localPlayer->IsValid()) {
        return -999999.0f;
    }

    // 使用缓存的最佳骨骼位置
    Vec3D targetPos = player->BestBonePosition;

    // 如果不合法（例如直接调用），尝试获取
    if (!targetPos.empty()) {
        bool forceHead = isKeyHeld("head");
        bool bonePositionValid = false;

        // 根据UI设置确定使用的骨骼
        int forceBone = -1;
        if (!aimParams.autoAimBone) {
            // 如果未启用自动选择骨骼，则强制使用UI指定的骨骼
            forceBone = aimParams.aimBone;
        }

        if (skeletonSystem) {
            bonePositionValid = skeletonSystem->GetBestBonePositionForAiming(player, targetPos, forceHead, forceBone);
        }

        if (!bonePositionValid) {
            HitboxType hitboxType = forceHead ? HitboxType::Head :
                (forceBone >= 0 ? static_cast<HitboxType>(forceBone) :
                    static_cast<HitboxType>(GetBestBone(player)));

            targetPos = GetBonePosition(player, hitboxType);
            if (!targetPos.empty()) {
                return -999999.0f;
            }
        }
    }

    float currentFOV = CalculateDistanceFromCrosshair(targetPos);

    const float BASE_SCORE = 100.0f;
    float score = 0.0f;

    // 根据优先级模式调整评分权重
    float fovWeight = 1.0f;
    float distanceWeight = 1.0f;
    float healthWeight = 0.5f;
    float threatWeight = 0.0f;

    switch (targetSettings.priorityMode) {
    case 0: // 距离优先
        distanceWeight = 3.0f;
        fovWeight = 1.0f;
        break;
    case 1: // FOV优先
        fovWeight = 3.0f;
        distanceWeight = 1.0f;
        break;
    case 2: // 血量优先
        healthWeight = 2.0f;
        break;
    case 3: // 威胁等级优先
        threatWeight = 2.0f;
        // 可以添加威胁等级评估逻辑
        break;
    }

    // FOV评分 (越接近中心分数越高)
    float fovRatio = currentFOV / bestFov;
    float fovScore = 1.0f - powf(fovRatio, 1.8f);
    score += fovScore * BASE_SCORE * fovWeight;

    // 距离评分 (近距离目标优先)
    float distanceRatio = player->Distance2DToLocalPlayer / aimParams.maxDistance;
    float distanceScore = 1.0f - powf(distanceRatio, 1.2f);
    score += distanceScore * BASE_SCORE * distanceWeight;

    // 可见性加分
    if (player->IsVisible) {
        score += 35.0f;
    }

    // 当前目标的持续性奖励
    if (player == targetSystem.target) {
        score += 20.0f;
    }

    // 血量越低分数越高
    float healthRatio = player->Health / 100.0f;
    score += (1.0f - healthRatio) * BASE_SCORE * healthWeight;

    //// 威胁评分 (例如，如果玩家正在瞄准你或者最近造成了伤害)
    //if (threatWeight > 0.0f && player->IsAimingAtYou) {
    //    score += 50.0f * threatWeight;
    //}

    return score;
}

int Aimbot::DetermineTargetType(Player* target) {
    // 确定目标类型: 0=所有目标, 1=普通玩家, 2=护盾玩家, 3=重装玩家
    int targetType = 0; // 默认为所有目标

    if (targetTypeSettings.currentTargetType != 0)
    {
        if (target->Shield > 0) {
            targetType = 2; // 护盾玩家
        }
        else {
            targetType = 1; // 普通玩家
        }
    }
    return targetType;
}

int Aimbot::GetBestBone(Player* target) {
    // 强制瞄头
    if (isKeyHeld("head")) return 0;

    // 自动选择部位
    if (aimParams.autoAimBone) {
        float nearestDistance = 999.0f;
        int bestBone = 1; // 默认瞄准颈部

        // 检查前4个骨骼点
        for (int i = 0; i < 4; i++) {
            float distance = CalculateDistanceFromCrosshair(
                GetBonePosition(target, static_cast<HitboxType>(i))
            );
            if (distance < nearestDistance) {
                bestBone = i;
                nearestDistance = distance;
            }
        }
        return bestBone;
    }

    // 根据目标类型返回特定的默认骨骼
    int targetType = DetermineTargetType(target);

    // 使用目标特定设置
    return targetTypeSettings.defaultBones[targetType];
}

int Aimbot::GetBoneFromHitbox(Player* p, HitboxType HitBox) {
    // 首先检查模型指针是否有效
    if (!mem.IsValidPointer(p->ModelPointer))
        return -1; // 如果模型指针无效，返回-1

    // 读取StudioHDR，这是模型数据的一个部分，包含了重要的模型信息
    uint64_t StudioHDR = mem.ReadCache<uint64_t>(p->ModelPointer + 0x8);
    // 检查StudioHDR指针的有效性
    if (!mem.IsValidPointer(StudioHDR + 0x34))
        return -1; // 如果无效，返回-1

    // 读取击中盒缓存信息
    auto HitboxCache = mem.ReadCache<uint16_t>(StudioHDR + 0x34);
    // 通过缓存信息计算出击中盒数组的地址
    auto HitboxArray = StudioHDR + (static_cast<uint64_t>(HitboxCache & 0xFFFE) << (4 * (HitboxCache & 1)));
    // 检查击中盒数组指针的有效性
    if (!mem.IsValidPointer(HitboxArray + 0x4))
        return -1; // 如果无效，返回-1

    // 读取索引缓存信息
    auto IndexCache = mem.ReadCache<uint16_t>(HitboxArray + 0x4);
    // 通过索引缓存信息计算出击中盒索引的地址
    auto HitboxIndex = ((uint16_t)(IndexCache & 0xFFFE) << (4 * (IndexCache & 1)));
    // 根据给定的HitBox类型计算出对应骨骼的指针
    auto BonePtr = HitboxIndex + HitboxArray + (static_cast<int>(HitBox) * 0x20);
    // 检查骨骼指针的有效性
    if (!mem.IsValidPointer(BonePtr))
        return -1; // 如果无效，返回-1

    // 读取并返回骨骼索引
    return mem.Read<uint16_t>(BonePtr);
}

Vec3D Aimbot::GetBonePosition(Player* p, HitboxType HitBox) {
    if (!p || !p->IsDataValid()) {
        return Vec3D();
    }

    Vec3D playerOrigin = p->Origin;
    if (playerOrigin.empty()) {
        return Vec3D();
    }

    int Bone = GetBoneFromHitbox(p, HitBox);
    if (Bone < 0 || Bone > 255) {
        return Vec3D();
    }

    uint64_t TempBonePointer = p->BonePointer + (Bone * sizeof(Matrix3x4));
    if (!mem.IsValidPointer(TempBonePointer)) {
        return Vec3D();
    }

    Matrix3x4 BoneMatrix = mem.Read<Matrix3x4>(TempBonePointer);
    Vec3D BonePosition = BoneMatrix.GetPosition();

    if (BonePosition.empty() ||
        std::isnan(BonePosition.x) || std::isnan(BonePosition.y) || std::isnan(BonePosition.z) ||
        std::isinf(BonePosition.x) || std::isinf(BonePosition.y) || std::isinf(BonePosition.z)) {
        return Vec3D();
    }

    Vec3D WorldPosition;
    WorldPosition.x = playerOrigin.x + BonePosition.x;
    WorldPosition.y = playerOrigin.y + BonePosition.y;
    WorldPosition.z = playerOrigin.z + BonePosition.z;

    if (WorldPosition.empty() ||
        abs(WorldPosition.x - playerOrigin.x) > 100.0f ||
        abs(WorldPosition.y - playerOrigin.y) > 100.0f ||
        abs(WorldPosition.z - playerOrigin.z) > 100.0f) {
        return playerOrigin;
    }

    return WorldPosition;
}

AimAngle Aimbot::CalculateAngle(Vec3D from, Vec3D to) {
    Vec3D delta = to - from;
    float length = delta.length();

    if (length < 0.0001f) {
        return AimAngle(0, 0);
    }

    // 使用常量而不是宏函数来转换弧度为角度
    float pitch = -asinf(delta.z / length) * RAD2DEG_CONST;
    float yaw = atan2f(delta.y, delta.x) * RAD2DEG_CONST;

    return AimAngle(pitch, yaw);
}

float Aimbot::CalculateDistanceFromCrosshair(Vec3D targetPos) {
    if (!localPlayer) return 999.0f;

    // Читаем состояние прицеливания
    bool isZooming = mem.Read<bool>(localPlayer->BasePointer + OFF_ZOOMING);

    // 计算带有后坐力补偿的当前视角
    float recoilX = localPlayer->AimPunch.x * (isZooming ?
        aimParams.scoped.recoilX : aimParams.noScope.recoilX);
    float recoilY = localPlayer->AimPunch.y * (isZooming ?
        aimParams.scoped.recoilY : aimParams.noScope.recoilY);

    // 创建带有后坐力的当前视角
    AimAngle currentAngle(
        localPlayer->ViewAngles.x + recoilX,
        localPlayer->ViewAngles.y + recoilY
    );

    // 计算目标角度
    AimAngle targetAngle = CalculateAngle(localPlayer->CameraOrigin, targetPos);

    // 返回两个角度之间的距离
    return currentAngle.DistanceTo(targetAngle);
}

// Add helper functions for Vec3D operations
inline float Vec3D_Distance(const Vec3D& a, const Vec3D& b) {
    return (a - b).length();
}

inline Vec3D Vec3D_Subtract(const Vec3D& a, const Vec3D& b) {
    return a - b;
}

inline float Vec3D_Length(const Vec3D& v) {
    return v.length();
}

inline bool Vec3D_Empty(const Vec3D& v) {
    return v.x == 0.0f && v.y == 0.0f && v.z == 0.0f;
}

// Add QAngle class definition
class QAngle {
public:
    float pitch;
    float yaw;
    
    QAngle(float p = 0.0f, float y = 0.0f) : pitch(p), yaw(y) {}
};

// Add AimbotPrediction class definition here
class AimbotPrediction {
private:
    // Cache structure for high-frequency updates
    struct PredictionCache {
        Vec3D lastTargetPos;
        Vec3D lastPredictedPos;
        Vec3D lastTargetVel;
        float lastBulletSpeed = 0.0f;
        float lastBulletGrav = 0.0f;
        float lastUpdateTime = 0.0f;
        float lastVelMagnitude = 0.0f;
        static constexpr float cacheTimeout = 0.0005f; // Increased to 2000Hz refresh rate
        static constexpr float posThreshold = 0.02f;   // Increased threshold for position changes
        static constexpr float velThreshold = 0.2f;    // Increased threshold for velocity changes
    };

public:
    Vec3D UnifiedPredict(const Vec3D& targetPos, const Vec3D& targetVel, float bulletSpeed, float bulletGrav) {
        static PredictionCache cache;
        float currentTime = static_cast<float>(GetTickCount64()) / 1000.0f;

        // Enhanced cache validation with optimized thresholds
        bool shouldUpdate = (Vec3D_Distance(cache.lastTargetPos, targetPos) > cache.posThreshold) || 
                          (abs(cache.lastBulletSpeed - bulletSpeed) > 0.02f) ||
                          (abs(cache.lastBulletGrav - bulletGrav) > 0.02f) ||
                          (Vec3D_Distance(targetVel, cache.lastTargetVel) > cache.velThreshold) ||
                          ((currentTime - cache.lastUpdateTime) >= cache.cacheTimeout);

        if (!shouldUpdate) {
            return cache.lastPredictedPos;
        }

        // Initialize prediction parameters
        Vec3D playerPos = localPlayer->CameraOrigin;
        float distance = Vec3D_Distance(playerPos, targetPos);
        float velMagnitude = Vec3D_Length(targetVel);

        // Optimized time-to-target calculation
        float timeToTarget = distance / (bulletSpeed * 1.15f);

        if (velMagnitude > 0.001f) {
            // Pre-calculate common values
            Vec3D dirToTarget = Vec3D_Subtract(targetPos, playerPos).Normalize();
            float cosTheta = (targetVel.x * dirToTarget.x + 
                            targetVel.y * dirToTarget.y + 
                            targetVel.z * dirToTarget.z) / velMagnitude;
            
            // Optimized multi-term time calculation
            float t0 = distance / bulletSpeed;
            float t1 = -(distance * velMagnitude * cosTheta) / (bulletSpeed * bulletSpeed * 0.92f);
            float t2 = (velMagnitude * velMagnitude * (5.8f * cosTheta * cosTheta - 1.0f)) / 
                      (2.0f * bulletSpeed * bulletSpeed);
            
            // Dynamic velocity-based blending with optimized thresholds
            float blendFactor = std::min(velMagnitude / 350.0f, 1.0f);
            
            // Simplified time calculation with fewer terms
            timeToTarget = t0 + 
                         t1 * blendFactor + 
                         t2 * blendFactor * blendFactor;
        }

        Vec3D predictedPos = targetPos;
        
        if (!Vec3D_Empty(targetVel) && timeToTarget > 0.001f) {
            // Apply prediction with optimized calculations
            predictedPos.x += targetVel.x * timeToTarget;
            predictedPos.y += targetVel.y * timeToTarget;
            predictedPos.z += targetVel.z * timeToTarget - (0.5f * bulletGrav * timeToTarget * timeToTarget);
        }

        // Update cache
        cache.lastTargetPos = targetPos;
        cache.lastPredictedPos = predictedPos;
        cache.lastBulletSpeed = bulletSpeed;
        cache.lastBulletGrav = bulletGrav;
        cache.lastUpdateTime = currentTime;
        cache.lastVelMagnitude = velMagnitude;
        cache.lastTargetVel = targetVel;

        return predictedPos;
    }

    // Helper function for calculating angles
    QAngle CalculateAngle(Vec3D from, Vec3D to) {
        Vec3D delta = Vec3D_Subtract(to, from);
        float length = Vec3D_Length(delta);
        
        if (length < 0.0001f) {
            return QAngle(0, 0);
        }

        float z = delta.z;
        float r = sqrt(delta.x * delta.x + delta.y * delta.y);
        
        // Dynamic threshold based on angle steepness
        float threshold = 0.95f + 0.04f * (abs(z) / (length + 0.0001f));
        
        float pitch;
        if (abs(z) < length * threshold) {
            float x = z / length;
            float x2 = x * x;
            float x3 = x2 * x;
            float x5 = x3 * x2;
            float x7 = x5 * x2;
            float x9 = x7 * x2;
            float x11 = x9 * x2;
            float x13 = x11 * x2;
            float x15 = x13 * x2;
            
            // Extended Taylor series for enhanced accuracy
            pitch = x 
                - (x3/6.0f) 
                + (3.0f*x5/40.0f) 
                - (5.0f*x7/112.0f) 
                + (35.0f*x9/1152.0f) 
                - (63.0f*x11/2816.0f)
                + (231.0f*x13/13312.0f)
                - (143.0f*x15/10240.0f);
                
            float correction = 1.0f + 0.001f * abs(x7);
            pitch *= -180.0f / M_PI * correction;
        } else {
            // Enhanced extreme angle calculation
            float angle = -asinf(std::clamp(z / length, -1.0f, 1.0f));
            pitch = angle * (180.0f / M_PI);
            
            if (abs(pitch) > 85.0f) {
                float factor = 1.0f + 0.02f * (abs(pitch) - 85.0f);
                pitch *= factor;
            }
        }

        // Calculate yaw with enhanced accuracy
        float yaw;
        if (abs(delta.x) > 0.0001f) {
            float y_x = delta.y / delta.x;
            if (abs(y_x) < 1.0f) {
                float y_x2 = y_x * y_x;
                float y_x3 = y_x2 * y_x;
                float y_x5 = y_x3 * y_x2;
                float y_x7 = y_x5 * y_x2;
                float y_x9 = y_x7 * y_x2;
                float y_x11 = y_x9 * y_x2;
                float y_x13 = y_x11 * y_x2;
                float y_x15 = y_x13 * y_x2;
                
                // Extended Taylor series for arctangent
                yaw = y_x 
                    - (y_x3/3.0f) 
                    + (y_x5/5.0f) 
                    - (y_x7/7.0f) 
                    + (y_x9/9.0f) 
                    - (y_x11/11.0f)
                    + (y_x13/13.0f)
                    - (y_x15/15.0f);
                    
                yaw *= 180.0f / M_PI;
                
                if (delta.x < 0.0f) {
                    yaw += (delta.y >= 0.0f) ? 180.0f : -180.0f;
                }
                
                if (abs(yaw) < 1.0f) {
                    yaw *= 1.0f + 0.001f * abs(yaw);
                }
            } else {
                yaw = atan2f(delta.y, delta.x) * (180.0f / M_PI);
                
                if (abs(abs(yaw) - 90.0f) < 5.0f) {
                    float sign = (yaw >= 0.0f) ? 1.0f : -1.0f;
                    yaw = sign * (90.0f + 0.1f * (abs(yaw) - 90.0f));
                }
            }
        } else {
            yaw = (delta.y >= 0.0f) ? 90.0f : -90.0f;
        }

        return QAngle(pitch, yaw);
    }
};

Vec3D Aimbot::CalculatePredictedPosition(Vec3D targetPos, Vec3D targetVel, float bulletSpeed, float bulletGrav) {
    // Debug removed: Target moving velocity
    if (!localPlayer || !localPlayer->IsValid() || !predictionSettings.enabled) {
        return targetPos;
    }

    static AimbotPrediction predictor;

    Vec3D result;
    if (predictionSettings.predictionSystem == 0) {
        result = targetPos;
        AimCorrection(localPlayer->CameraOrigin, &result, targetVel, bulletSpeed, bulletGrav);
    }
    else {
        result = predictor.UnifiedPredict(targetPos, targetVel, bulletSpeed, bulletGrav);
    }

    // Check if prediction actually changed the position
    bool predictionApplied = (abs(result.x - targetPos.x) > 0.1f ||
                             abs(result.y - targetPos.y) > 0.1f ||
                             abs(result.z - targetPos.z) > 0.1f);

    // Debug removed: Prediction success/failure messages

    return result;
}

inline float MeterToGameUnits(float Meters) {
    return 39.37007874 * Meters;
}
inline float GameUnitsToMeter(float GameUnits) {
    return  GameUnits / 39.37007874;
}

inline void SolveQuartic(const std::complex<float> coefficients[5], std::complex<float> roots[4]) {
    const std::complex<float> a = coefficients[4];
    const std::complex<float> b = coefficients[3] / a;
    const std::complex<float> c = coefficients[2] / a;
    const std::complex<float> d = coefficients[1] / a;
    const std::complex<float> e = coefficients[0] / a;

    const std::complex<float> Q1 = c * c - 3.f * b * d + 12.f * e;
    const std::complex<float> Q2 = 2.f * c * c * c - 9.f * b * c * d + 27.f * d * d + 27.f * b * b * e - 72.f * c * e;
    const std::complex<float> Q3 = 8.f * b * c - 16.f * d - 2.f * b * b * b;
    const std::complex<float> Q4 = 3.f * b * b - 8.f * c;

    const std::complex<float> Q5 = std::pow(Q2 / 2.f + std::sqrt(Q2 * Q2 / 4.f - Q1 * Q1 * Q1), 1.f / 3.f);
    const std::complex<float> Q6 = (Q1 / Q5 + Q5) / 3.f;
    const std::complex<float> Q7 = 2.f * std::sqrt(Q4 / 12.f + Q6);

    roots[0] = (-b - Q7 - std::sqrt(4.f * Q4 / 6.f - 4.f * Q6 - Q3 / Q7)) / 4.f;
    roots[1] = (-b - Q7 + std::sqrt(4.f * Q4 / 6.f - 4.f * Q6 - Q3 / Q7)) / 4.f;
    roots[2] = (-b + Q7 - std::sqrt(4.f * Q4 / 6.f - 4.f * Q6 + Q3 / Q7)) / 4.f;
    roots[3] = (-b + Q7 + std::sqrt(4.f * Q4 / 6.f - 4.f * Q6 + Q3 / Q7)) / 4.f;
}
inline void Aimbot::AimCorrection(Vec3D LocalPos, Vec3D* InVecArg, Vec3D currVelocity, float Bulletspeed, float Bulletgrav)
{
    // Validate input parameters
    if (Bulletspeed <= 0 || Bulletgrav <= 0 || !std::isfinite(Bulletspeed) || !std::isfinite(Bulletgrav)) {
        std::cout << "[AimCorrection] ERROR: Invalid weapon parameters - Speed: " << Bulletspeed << ", Grav: " << Bulletgrav << std::endl;
        return;
    }

    std::cout << "[AimCorrection] === STARTING CALCULATION ===" << std::endl;
    std::cout << "[AimCorrection] LocalPos: (" << LocalPos.x << "," << LocalPos.y << "," << LocalPos.z << ")" << std::endl;
    std::cout << "[AimCorrection] TargetPos: (" << InVecArg->x << "," << InVecArg->y << "," << InVecArg->z << ")" << std::endl;
    std::cout << "[AimCorrection] Velocity: (" << currVelocity.x << "," << currVelocity.y << "," << currVelocity.z << ")" << std::endl;
    std::cout << "[AimCorrection] BulletSpeed: " << Bulletspeed << ", Grav: " << Bulletgrav << std::endl;

    double G = GameUnitsToMeter(750.0f / Bulletgrav);
    double A = GameUnitsToMeter(LocalPos.y);
    double B = GameUnitsToMeter(LocalPos.z);
    double C = GameUnitsToMeter(LocalPos.x);
    double M = GameUnitsToMeter(InVecArg->y);
    double N = GameUnitsToMeter(InVecArg->z);
    double O = GameUnitsToMeter(InVecArg->x);
    double P = GameUnitsToMeter(currVelocity.y);
    double Q = GameUnitsToMeter(currVelocity.z);
    double R = GameUnitsToMeter(currVelocity.x);
    double S = GameUnitsToMeter(Bulletspeed);
    double H = M - A;
    double J = O - C;
    double K = N - B;
    double L = -.5f * G;
    double c4 = L * L;
    double c3 = -2 * Q * L;
    double c2 = (Q * Q) - (2 * K * L) - (S * S) + (P * P) + (R * R);
    double c1 = (2 * K * Q) + (2 * H * P) + (2 * J * R);
    double c0 = (K * K) + (H * H) + (J * J);

    std::complex<float> pOutRoots[4];
    const std::complex<float> pInCoeffs[5] = { c0, c1, c2, c3, c4 };
    SolveQuartic(pInCoeffs, pOutRoots);
    float fBestRoot = FLT_MAX;
    bool foundValidRoot = false;
    for (int i = 0; i < 4; i++) {
        if (pOutRoots[i].real() > 0.f && std::abs(pOutRoots[i].imag()) < 0.0001f && pOutRoots[i].real() < fBestRoot) {
            fBestRoot = pOutRoots[i].real();
            foundValidRoot = true;
        }
    }

    if (!foundValidRoot) {
        std::cout << "[AimCorrection] ERROR: No valid root found! Using simple prediction instead." << std::endl;
        std::cout << "[AimCorrection] Roots found: " << (pOutRoots[0].real() > 0 ? "valid" : "invalid") << ", "
                  << (pOutRoots[1].real() > 0 ? "valid" : "invalid") << ", "
                  << (pOutRoots[2].real() > 0 ? "valid" : "invalid") << ", "
                  << (pOutRoots[3].real() > 0 ? "valid" : "invalid") << std::endl;

        // Fallback to simple prediction when AimCorrection fails
        Vec3D simplePrediction = *InVecArg;
        float predictionTime = 0.15f; // 150ms flight time for fallback
        simplePrediction.x += currVelocity.x * predictionTime;
        simplePrediction.y += currVelocity.y * predictionTime;
        simplePrediction.z += currVelocity.z * predictionTime;

        *InVecArg = simplePrediction;
        std::cout << "[AimCorrection] Applied simple prediction fallback" << std::endl;
        return;
    }
    InVecArg->y = MeterToGameUnits(A + (H + P * fBestRoot));
    InVecArg->z = MeterToGameUnits(B + (K + Q * fBestRoot - L * fBestRoot * fBestRoot));
    InVecArg->x = MeterToGameUnits(C + (J + R * fBestRoot));

    std::cout << "[AimCorrection] SUCCESS: PredictedPos: (" << InVecArg->x << "," << InVecArg->y << "," << InVecArg->z << ")" << std::endl;
    std::cout << "[AimCorrection] BestRoot: " << fBestRoot << ", Flight time: " << fBestRoot << " seconds" << std::endl;
    std::cout << "[AimCorrection] === CALCULATION COMPLETE ===" << std::endl;
}

void Aimbot::CheckAndTriggerTarget(Player* target, bool isMagnetTrigger) {
    isTriggering = false;

    if (!target || !target->IsDataValid() || !localPlayer || !localPlayer->IsValid()) {
        // std::cout << "[TriggerBot] Invalid target or player data" << std::endl;
        return;
    }

    // For magnet trigger, skip AimedAt check since we're aiming at prediction
    if (!isMagnetTrigger) {
        // Update IsAimedAt for the target before checking ShouldTrigger
        target->AimedAtCheck();

        // Debug: check if target is aimed at
        // std::cout << "[TriggerBot] Target IsAimedAt: " << (target->IsAimedAt ? "YES" : "NO") << std::endl;
    }

    // 简化后直接调用TriggerBot的ShouldTrigger方法
    bool shouldTrigger = triggerbot->ShouldTrigger(target);

    if (shouldTrigger) {
        // std::cout << "[TriggerBot] Triggering shot at target" << std::endl;

        // Отладка: проверим состояние цели перед выстрелом
        // std::cout << "[TriggerBot] Before shot - Target IsAimedAt: " << (target->IsAimedAt ? "YES" : "NO")
        //           << ", LastTimeAimedAt: " << target->LastTimeAimedAt
        //           << ", LastTimeAimedAtPrevious: " << target->LastTimeAimedAtPrevious << std::endl;

        kmbox->LeftClick();
        triggerbot->UpdateLastClickTime();
        isTriggering = true;

        // Отладка: проверим состояние после выстрела
        target->AimedAtCheck(); // Обновим статус
        // std::cout << "[TriggerBot] After shot - Target IsAimedAt: " << (target->IsAimedAt ? "YES" : "NO")
        //           << ", LastTimeAimedAt: " << target->LastTimeAimedAt
        //           << ", LastTimeAimedAtPrevious: " << target->LastTimeAimedAtPrevious << std::endl;
    } else {
        // Log why not triggering occasionally
        // static auto lastNoTriggerLog = std::chrono::steady_clock::now();
        // auto now = std::chrono::steady_clock::now();
        // if (std::chrono::duration_cast<std::chrono::seconds>(now - lastNoTriggerLog).count() >= 3) {
        //     std::cout << "[TriggerBot] Not triggering - conditions not met" << std::endl;
        //     lastNoTriggerLog = now;
        // }
    }
}


Vec2D Aimbot::ApplySensitivityCorrection(Vec2D step) {

    const float defaultSensitivity = 5.0f;

    // 防止除零错误或异常值
    if (localPlayer->mouseSensitive <= 0.1f) {
        localPlayer->mouseSensitive = 0.1f;
    }

    // 当灵敏度高时，移动量需要变小；当灵敏度低时，移动量需要变大
    float adjustmentFactor = defaultSensitivity / localPlayer->mouseSensitive;

    // 限制调整因子在合理范围内
    adjustmentFactor = std::clamp(adjustmentFactor, 0.1f, 10.0f);

    // 应用调整
    return {
        step.x * adjustmentFactor,
        step.y * adjustmentFactor
    };
}

void Aimbot::StartAiming(Player* target) {
    if (!target || !target->IsDataValid() || !localPlayer || !localPlayer->IsValid()) {
        // std::cout << "[AimBot] StartAiming failed - invalid target or player" << std::endl;
        return;
    }

    // Читаем состояние прицеливания
    bool isZooming = mem.Read<bool>(localPlayer->BasePointer + OFF_ZOOMING);

    // std::cout << "[AimBot] StartAiming called for target" << std::endl;

    // 计算当前FOV相关参数
    // FOV не меняется от кратности прицела
    float scopedFov = aimParams.scoped.fov;
    const float maxFov = isZooming ? scopedFov : aimParams.noScope.fov;
    const float fovScale = GetFOVScale();
    float bestFov = std::min(maxFov, maxFov * (fovScale * 1.2f));

    float currentTime = static_cast<float>(GetTickCount64()) / 1000.0f;
    AimAngle currentAngle(localPlayer->ViewAngles.x, localPlayer->ViewAngles.y);

    // 获取后坐力值
    float currentRecoilX = localPlayer->AimPunch.x * (isZooming ?
        aimParams.scoped.recoilX : aimParams.noScope.recoilX);
    float currentRecoilY = localPlayer->AimPunch.y * (isZooming ?
        aimParams.scoped.recoilY : aimParams.noScope.recoilY);


    // 判断是否是新目标
    bool isNewTarget = (targetSystem.target != target);
    if (isNewTarget) {
        targetSystem.lastSwitchTime = GetTickCount64() / 1000.0f;
        targetSystem.target = target;
        humanParams.bezierProgress = 0.0f;

    }
    float baseSmooth = isZooming ? aimParams.scoped.smooth : aimParams.noScope.smooth;
    float smoothFactorX = baseSmooth;
    float smoothFactorY = baseSmooth;

    // 计算切换后经过的时间
    float timeSinceSwitch = currentTime - targetSystem.lastSwitchTime;

    // 如果在切换平滑持续时间内，应用额外平滑
    if (timeSinceSwitch < targetSystem.switchSmoothDuration) {
        // 平滑倍数随时间线性减少
        float progress = timeSinceSwitch / targetSystem.switchSmoothDuration;

        // 修改为基于当前平滑值的比例
        // 例如：如果switchSmoothScale=2.0，则初始平滑值加倍
        float additionalSmooth = baseSmooth * (aimParams.switchSmoothScale - 1.0f) * (1.0f - progress);
        additionalSmooth /= 2;
        // 应用额外平滑
        smoothFactorX += additionalSmooth;
        smoothFactorY += additionalSmooth;
    }

    // *** 关键修改：每次StartAiming都重新获取最新的骨骼位置 ***
    Vec3D targetBonePos;
    bool bonePositionValid = false;

    // 缓存强制瞄头状态
    bool forceHead = isKeyHeld("head");

    // 根据UI设置确定使用的骨骼
    int forceBone = -1;
    if (!aimParams.autoAimBone) {
        // 如果未启用自动选择骨骼，则强制使用UI指定的骨骼
        forceBone = aimParams.aimBone;
    }

    // 从Skeleton系统获取最新骨骼位置
    if (skeletonSystem) {
        bonePositionValid = skeletonSystem->GetBestBonePositionForAiming(target, targetBonePos, forceHead, forceBone);
    }

    // 如果需要，回退到原始方法
    if (!bonePositionValid) {
        HitboxType hitboxType = forceHead ? HitboxType::Head :
            (forceBone >= 0 ? static_cast<HitboxType>(forceBone) :
                static_cast<HitboxType>(GetBestBone(target)));

        targetBonePos = GetBonePosition(target, hitboxType);
        if (targetBonePos.empty()) {
            // 如果获取失败，使用玩家位置加上偏移
            targetBonePos = target->Origin;
            targetBonePos.z += 50.0f;
        }
    }

    // 更新缓存的最佳骨骼位置
    target->BestBonePosition = targetBonePos;

    // 检查是否在FOV范围内
    float currentFOV = CalculateDistanceFromCrosshair(targetBonePos);
    if (currentFOV > bestFov) {
        ReleaseTarget();
        return;
    }

    // 获取目标位置并应用预测
    Vec3D localPos = localPlayer->CameraOrigin;

    // 实时计算预测位置
    // Declare predictedPos variable
    Vec3D predictedPos;

    // Check for invalid weapon data or disabled prediction
    if (!predictionSettings.enabled) {
        std::cout << "[Prediction] DISABLED - using target position directly" << std::endl;
        predictedPos = targetBonePos;
    }
    else {
        // Additional safety checks for prediction
        bool hasValidWeaponData = localPlayer->WeaponProjectileSpeed > 100 &&  // Minimum reasonable speed for Apex
                                  localPlayer->WeaponProjectileSpeed < 50000 && // Maximum reasonable speed
                                  std::isfinite(localPlayer->WeaponProjectileSpeed) &&
                                  localPlayer->WeaponProjectileScale > 0.1 &&
                                  localPlayer->WeaponProjectileScale < 5.0 && // Reasonable gravity scale for Apex
                                  std::isfinite(localPlayer->WeaponProjectileScale);

        bool targetIsMoving = abs(target->AbsoluteVelocity.x) > 0.1f ||
                             abs(target->AbsoluteVelocity.y) > 0.1f ||
                             abs(target->AbsoluteVelocity.z) > 0.1f;

        // Debug removed: Weapon data check

        if (!targetIsMoving) {
            // Debug removed: Skipping - target not moving enough
            predictedPos = targetBonePos;
        }
        else if (!hasValidWeaponData) {
            std::cout << "[Prediction] Invalid weapon data, using simple prediction" << std::endl;
            // Simple prediction for invalid weapon data - just add velocity * time
            float predictionTime = 0.2f; // Assume 200ms flight time
            predictedPos.x = targetBonePos.x + target->AbsoluteVelocity.x * predictionTime;
            predictedPos.y = targetBonePos.y + target->AbsoluteVelocity.y * predictionTime;
            predictedPos.z = targetBonePos.z + target->AbsoluteVelocity.z * predictionTime;
            // Debug removed: Simple prediction applied
        }
        else {
            // Debug removed: Calculating full ballistic prediction
            predictedPos = CalculatePredictedPosition(
                targetBonePos,
                target->AbsoluteVelocity,
                localPlayer->WeaponProjectileSpeed,
                localPlayer->WeaponProjectileScale
            );
        }
    }

    // Check if predicted position is valid
    if (!std::isfinite(predictedPos.x) || !std::isfinite(predictedPos.y) || !std::isfinite(predictedPos.z)) {
        predictedPos = targetBonePos;
    }

    // Apply prediction scale
    if (predictionSettings.scale != 1.0f) {
        Vec3D predictionDiff = predictedPos - targetBonePos;
        predictedPos = targetBonePos + predictionDiff * predictionSettings.scale;
        std::cout << "[Prediction] Applied scale " << predictionSettings.scale << " to prediction" << std::endl;
    }

    // 计算目标角度
    AimAngle targetAngle = CalculateAngle(localPos, predictedPos);


    // 检查目标角度变化，重大变化时重置贝塞尔曲线
    float targetAngleDelta = humanParams.lastTargetAngle.DistanceTo(targetAngle);
    if (targetAngleDelta > 5.0f) {
        humanParams.bezierProgress = 0.0f;
    }
    humanParams.lastTargetAngle = targetAngle;

    // FOV检查
    float fov = currentAngle.DistanceTo(targetAngle);
    if (fov > maxFov) return;

    if (localPlayer->InAttack) {
        smoothFactorX *= aimParams.shootingSmoothScale;
        smoothFactorY *= aimParams.shootingSmoothScale;
    }

    // 计算考虑后坐力的目标角度
    AimAngle recoilCompensatedTargetAngle = targetAngle;

    // 应用后坐力补偿到目标角度（反向补偿，所以是减法）
    recoilCompensatedTargetAngle.pitch -= currentRecoilX;
    recoilCompensatedTargetAngle.yaw -= currentRecoilY;
    recoilCompensatedTargetAngle.Normalize();

    // 声明最终角度变量
    AimAngle finalAngle;

    if (!humanParams.enabled || (humanParams.humanizeMode == 1 &&
        (humanParams.nonLinearPathStrength <= 0.f ||
            (humanParams.type == 1 && !targetSystem.target->IsAimedAt) ||
            (humanParams.type == 2 && !localPlayer->InAttack)))) {

        // 线性瞄准方法 - 保持原有逻辑但使用压枪后的目标角度
        // 计算角度差异
        AimAngle deltaAngle = recoilCompensatedTargetAngle - currentAngle;

        // 确保角度差在正确范围内
        if (deltaAngle.yaw > 180.0f) deltaAngle.yaw -= 360.0f;
        else if (deltaAngle.yaw < -180.0f) deltaAngle.yaw += 360.0f;

        if (deltaAngle.pitch > 180.0f) deltaAngle.pitch -= 360.0f;
        else if (deltaAngle.pitch < -180.0f) deltaAngle.pitch += 360.0f;

        // 应用平滑
        AimAngle smoothedDelta;
        smoothedDelta.pitch = deltaAngle.pitch / std::max(smoothFactorY, 1.0f);
        smoothedDelta.yaw = deltaAngle.yaw / std::max(smoothFactorX, 1.0f);

        // 计算最终角度
        finalAngle = currentAngle + smoothedDelta;
    }
    else {
        // 根据选择的模式应用不同的人性化处理
        if (humanParams.humanizeMode == 0) {
            // 模式1: 曲线偏移方法

            // 将角度转换为向量差
            Vec3D currentVec = localPos;
            Vec3D targetVec = predictedPos;

            // 应用人性化处理
            Vec3D humanizedTarget = ApplyHumanizationToAim(targetVec, currentVec, 0);

            // 计算新的目标角度
            AimAngle humanizedAngle = CalculateAngle(localPos, humanizedTarget);

            // 应用后坐力补偿
            humanizedAngle.pitch -= currentRecoilX;
            humanizedAngle.yaw -= currentRecoilY;
            humanizedAngle.Normalize();

            // 计算从当前到人性化角度的差值
            AimAngle deltaAngle = humanizedAngle - currentAngle;

            // 确保角度差在正确范围内
            if (deltaAngle.yaw > 180.0f) deltaAngle.yaw -= 360.0f;
            else if (deltaAngle.yaw < -180.0f) deltaAngle.yaw += 360.0f;

            if (deltaAngle.pitch > 180.0f) deltaAngle.pitch -= 360.0f;
            else if (deltaAngle.pitch < -180.0f) deltaAngle.pitch += 360.0f;

            // 应用平滑
            AimAngle smoothedDelta;
            smoothedDelta.pitch = deltaAngle.pitch / std::max(smoothFactorY, 1.0f);
            smoothedDelta.yaw = deltaAngle.yaw / std::max(smoothFactorX, 1.0f);

            // 计算最终角度
            finalAngle = currentAngle + smoothedDelta;
        }
        else {
            // 模式2: 贝塞尔曲线方法(原有的实现)
            // **** 贝塞尔曲线瞄准方法 ****
            // 创建控制点 - 在当前和目标角度之间找一个合适的中间点
            AimAngle controlPoint;
            // 计算角度差值，确保处理越过180/-180度边界的情况
            float pitchDiff = recoilCompensatedTargetAngle.pitch - currentAngle.pitch;
            if (pitchDiff > 180.0f) pitchDiff -= 360.0f;
            else if (pitchDiff < -180.0f) pitchDiff += 360.0f;

            float yawDiff = recoilCompensatedTargetAngle.yaw - currentAngle.yaw;
            if (yawDiff > 180.0f) yawDiff -= 360.0f;
            else if (yawDiff < -180.0f) yawDiff += 360.0f;

            // 计算控制点位置 - 使用更可靠的方法，不会导致方向反转
            float midPitch = currentAngle.pitch + pitchDiff * 0.5f;
            float midYaw = currentAngle.yaw + yawDiff * 0.5f;

            // 修改：控制点偏移量 - 减小默认值，避免过度偏移
            float controlDeviation = 0.5f * humanParams.nonLinearPathStrength;

            // 使用更安全的随机数生成
            static std::random_device rd;
            static std::mt19937 gen(rd());
            static std::uniform_real_distribution<float> dis(-1.0f, 1.0f);

            // 计算垂直于移动方向的偏移方向
            float dirLength = sqrtf(pitchDiff * pitchDiff + yawDiff * yawDiff);
            if (dirLength > 0.001f) {
                // 标准化差异向量
                float normPitchDiff = pitchDiff / dirLength;
                float normYawDiff = yawDiff / dirLength;

                // 创建垂直方向(-y, x)
                float perpPitch = -normYawDiff;
                float perpYaw = normPitchDiff;

                // 垂直偏移 = 正交向量 * 随机大小
                float randOffset = dis(gen) * controlDeviation;

                // 应用偏移到中点
                controlPoint.pitch = midPitch + perpPitch * randOffset;
                controlPoint.yaw = midYaw + perpYaw * randOffset;
            }
            else {
                // 差异太小时不应用偏移
                controlPoint.pitch = midPitch;
                controlPoint.yaw = midYaw;
            }

            // 确保控制点角度在有效范围内
            controlPoint.Normalize();

            // ==== 修改进度更新 - 确保与smooth值关联正确 ====
            // 更新贝塞尔曲线进度，确保与smooth值成正比
            // 减小基础进度增量，提供更平滑的过渡
            float baseProgressInc = 0.008f;

            // 平滑度的平方根关系，使得高smooth值有更小的增量
            float smoothScale = 1.0f / sqrtf(std::max(smoothFactorX, 1.0f));

            // 设定最小和最大增量，防止极端情况
            float minProgressInc = 0.003f;
            float maxProgressInc = 0.02f;

            float progressInc = std::min(std::max(baseProgressInc * smoothScale, minProgressInc), maxProgressInc);

            // 考虑bezierSpeed作为用户可调整的速度修正
            progressInc *= humanParams.bezierSpeed;

            // 更新进度
            humanParams.bezierProgress += progressInc;

            // 限制最大进度
            if (humanParams.bezierProgress > 1.0f) {
                humanParams.bezierProgress = 1.0f;
            }

            // 计算贝塞尔曲线点
            finalAngle = CalculateBezierPoint(currentAngle, controlPoint, recoilCompensatedTargetAngle, humanParams.bezierProgress);

            // 微调功能已移除 - ApplyMicroAdjustments function removed
        }
    }

    // 归一化最终角度
    finalAngle.Normalize();

    // Memory write removed - using only KmBox method
    // Only KmBox method available
    // std::cout << "[AimBot] Using KmBox method" << std::endl;

        Vec2D screenPos;
        if (!gameView->WorldToScreen(predictedPos, screenPos)) {
            // Only log WorldToScreen failures occasionally
            // static auto lastWTSFailLog = std::chrono::steady_clock::now();
            // auto now = std::chrono::steady_clock::now();
            // if (std::chrono::duration_cast<std::chrono::seconds>(now - lastWTSFailLog).count() >= 1) {
            //     std::cout << "[AimBot] Target not visible on screen (behind, too far, etc.)" << std::endl;
            //     lastWTSFailLog = now;
            // }
            return;
        }

        // std::cout << "[AimBot] WorldToScreen success - screenPos: (" << screenPos.x << ", " << screenPos.y << ")" << std::endl;

        Vec2D screenCenter(gameView->ScreenCenter.x, gameView->ScreenCenter.y);
        float screenHeight = gameView->ScreenSize.y;

        // 计算基础移动 - 这是原始版本的方法，直接使用屏幕坐标差
        Vec2D baseMovement = {
            screenPos.x - screenCenter.x,
            screenPos.y - screenCenter.y
        };

        // std::cout << "[AimBot] Base movement: (" << baseMovement.x << ", " << baseMovement.y << ")" << std::endl;

        baseMovement = ApplySensitivityCorrection(baseMovement);
        baseMovement.x /= smoothFactorX;
        baseMovement.y /= smoothFactorY;

        // std::cout << "[AimBot] Final movement: (" << baseMovement.x << ", " << baseMovement.y << ")" << std::endl;
        // std::cout << "[AimBot] Using KmBox: move(" << static_cast<int>(baseMovement.x) << ", " << static_cast<int>(baseMovement.y) << ")" << std::endl;
        kmbox->Move(static_cast<int>(baseMovement.x), static_cast<int>(baseMovement.y));
    }

// void Aimbot::ApplyMicroAdjustments(AimAngle& angle) {
//     if (!humanParams.enabled || humanParams.microAdjustScale <= 0.0f)
//         return;

//     // 修复条件判断：确保目标指针有效且目标正在瞄准玩家时不应用微调
//     if (targetSystem.target != nullptr && targetSystem.target->IsAimedAt) {
//         return;
//     }

    // // 使用更安全的随机数生成
    // static std::random_device rd;
    // static std::mt19937 gen(rd());
    // static std::uniform_real_distribution<float> dis(-1.0f, 1.0f);

    // // 降低微调幅度，将原来的随机范围缩小到原来的一半
    // float adjustScale = humanParams.microAdjustScale * 0.5f;

    // // 垂直方向抖动
    // if (humanParams.enablePitchAdjust) {
    //     float randOffsetPitch = dis(gen) * adjustScale;
    //     angle.pitch += randOffsetPitch;
    // }

    // // 水平方向抖动
    // if (humanParams.enableYawAdjust) {
    //     float randOffsetYaw = dis(gen) * adjustScale;
    //     angle.yaw += randOffsetYaw;
    // }

    // // 确保角度在合理范围内
    // angle.Normalize();
// }
AimAngle Aimbot::CalculateBezierPoint(const AimAngle& start, const AimAngle& control, const AimAngle& end, float t) {
    // 防止t超出0-1范围
    t = std::max(0.0f, std::min(1.0f, t));

    float oneMinusT = 1.0f - t;
    float oneMinusTSquared = oneMinusT * oneMinusT;
    float tSquared = t * t;

    AimAngle result;

    // 处理pitch方向的计算
    float startPitch = start.pitch;
    float controlPitch = control.pitch;
    float endPitch = end.pitch;

    // 确保角度差在有效范围内
    if (abs(controlPitch - startPitch) > 180.0f) {
        if (controlPitch > startPitch) controlPitch -= 360.0f;
        else controlPitch += 360.0f;
    }

    if (abs(endPitch - startPitch) > 180.0f) {
        if (endPitch > startPitch) endPitch -= 360.0f;
        else endPitch += 360.0f;
    }

    // 处理yaw方向的计算
    float startYaw = start.yaw;
    float controlYaw = control.yaw;
    float endYaw = end.yaw;

    // 确保角度差在有效范围内
    if (abs(controlYaw - startYaw) > 180.0f) {
        if (controlYaw > startYaw) controlYaw -= 360.0f;
        else controlYaw += 360.0f;
    }

    if (abs(endYaw - startYaw) > 180.0f) {
        if (endYaw > startYaw) endYaw -= 360.0f;
        else endYaw += 360.0f;
    }

    // 使用二次贝塞尔插值公式
    result.pitch = oneMinusTSquared * startPitch + 2.0f * oneMinusT * t * controlPitch + tSquared * endPitch;
    result.yaw = oneMinusTSquared * startYaw + 2.0f * oneMinusT * t * controlYaw + tSquared * endYaw;

    // 确保结果在有效范围内
    result.Normalize();

    return result;
}
bool Aimbot::ShouldUpdateAim() {
    if (!humanParams.enabled)
        return true;

    float currentTime = static_cast<float>(GetTickCount64()) / 1000.0f;

    // 检查是否到达下一次更新时间
    if (currentTime < humanParams.lastUpdateTime + humanParams.nextUpdateDelay) {
        return false;
    }

    // 更新时间和下一次延迟
    humanParams.lastUpdateTime = currentTime;

    // 生成随机延迟 - 减小延迟变化来避免对平滑度的过度干扰
    // 使用更安全的随机数生成
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<float> dis(0.0f, 1.0f);

    // 减少随机因素，使延迟更加稳定
    float baseDelay = 0.004f; // 降低基础延迟
    float maxRandomization = 0.005f; // 减小随机化程度

    // 修改：更窄的时间变化范围
    float minVar = std::min(humanParams.timeVariationMin, 0.8f);
    float maxVar = std::max(humanParams.timeVariationMax, 1.2f);

    // 生成更稳定的延迟
    humanParams.nextUpdateDelay = baseDelay + dis(gen) * maxRandomization * (maxVar - minVar) + minVar * 0.005f;

    return true;
}
void Aimbot::ReleaseTarget() {
    // Log only when target was actually released
    if (targetSystem.target != nullptr) {
        // static auto lastReleaseLog = std::chrono::steady_clock::now();
        // auto now = std::chrono::steady_clock::now();
        // if (std::chrono::duration_cast<std::chrono::milliseconds>(now - lastReleaseLog).count() >= 1000) {
        //     std::cout << "[AimBot] Releasing target" << std::endl;
        //     lastReleaseLog = now;
        // }
    }

    targetSystem.target = nullptr;
    // skyNadePointCache removed - skynade functionality disabled
}

float Aimbot::GetFOVScale() const {
    // FOV не зависит от кратности прицела - всегда возвращаем 1.0
    return 1.0f;
}

bool Aimbot::isKeyHeld(const std::string& keyName) {
    return IS_KEY_HELD("Aimbot", keyName);
}

void Aimbot::RenderDraw() {
    if (!entityManager.isInGame() || !localPlayer || !localPlayer->IsValid() || localPlayer->IsEmptyHand) {
        return;
    }

    

    float centerX = gameView->ScreenCenter.x;
    float centerY = gameView->ScreenCenter.y;
    auto& renderer = ImGuiRenderer::getInstance();
    ImDrawList* drawList = ImGui::GetBackgroundDrawList();


    // SkyNade rendering removed - functionality disabled
    {
        if (aimParams.showFOV)
        {
            // FOV не меняется от кратности прицела для рисования круга
            bool isZooming = mem.Read<bool>(localPlayer->BasePointer + OFF_ZOOMING);
            float currentFOV = isZooming ? aimParams.scoped.fov : aimParams.noScope.fov;

            // FOV角度转换为屏幕像素距离
            float screenFOV = tanf(DEG2RAD(currentFOV)) * (gameView->ScreenSize.y / 2);

            // 根据缩放调整半径
            float radius = screenFOV;
            if (isZooming) {
                float fovScale = GetFOVScale();
                radius *= fovScale;
            }

            // Отладка FOV круга
            // static int fovDebugCounter = 0;
            // if (fovDebugCounter++ % 300 == 0) { // Показывать каждый 300-й кадр
            //     std::cout << "[FOV] isZooming: " << isZooming
            //              << ", currentFOV: " << currentFOV
            //              << ", screenFOV: " << screenFOV
            //              << ", radius: " << radius
            //              << ", center: (" << centerX << ", " << centerY << ")"
            //              << ", screenSize: (" << gameView->ScreenSize.x << ", " << gameView->ScreenSize.y << ")" << std::endl;
            // }

            // 根据目标状态选择颜色
            ImColor circleColor;
            if (targetSystem.target && IsValidTarget(targetSystem.target)) {
                // 使用UI设置的锁定目标颜色
                circleColor = ImColor(
                    visualSettings.targetColor.x,
                    visualSettings.targetColor.y,
                    visualSettings.targetColor.z,
                    visualSettings.targetColor.w
                );
            }
            else {
                // 使用UI设置的正常状态颜色
                circleColor = ImColor(
                    visualSettings.normalColor.x,
                    visualSettings.normalColor.y,
                    visualSettings.normalColor.z,
                    visualSettings.normalColor.w
                );
            }

            // 绘制FOV圆圈
            renderer.drawCircle(ImVec2(centerX, centerY), radius, circleColor, 60, aimParams.fovThickness);
        }

        RenderTriggerFrame();

        if (humanParams.showLockIndicator && !isKeyHeld("trigger") && targetSystem.target && IsValidTarget(targetSystem.target)) {
            Vec2D targetScreenPos;
            if (gameView->WorldToScreen(targetSystem.target->BestBonePosition, targetScreenPos)) {
                renderer.drawLine({ centerX,centerY }, { targetScreenPos.x,targetScreenPos.y }, humanParams.indicatorColor, 1.5f);
            }
        }
    }

    
}

void Aimbot::RenderPredictionDebug() {
    // Only render debug if explicitly enabled and we have a valid target
    if (!predictionSettings.showDebug || !predictionSettings.enabled ||
        !targetSystem.target || !IsValidTarget(targetSystem.target) ||
        !localPlayer || !localPlayer->IsValid() ||
        (isSkyNadeActive && localPlayer->IsHoldingGrenade)) {
        return;
    }

    // 获取目标骨骼位置
    Vec3D targetBonePos = targetSystem.target->BestBonePosition;
    if (targetBonePos.empty()) {
        // 如果没有缓存的位置，尝试获取
        bool forceHead = isKeyHeld("head");
        int forceBone = !aimParams.autoAimBone ? aimParams.aimBone : -1;

        bool bonePositionValid = false;
        if (skeletonSystem) {
            bonePositionValid = skeletonSystem->GetBestBonePositionForAiming(
                targetSystem.target, targetBonePos, forceHead, forceBone);
        }

        if (!bonePositionValid) {
            return;
        }
    }

    // 计算预测位置
    Vec3D predictedPos = CalculatePredictedPosition(
        targetBonePos,
        targetSystem.target->AbsoluteVelocity,
        localPlayer->WeaponProjectileSpeed,
        localPlayer->WeaponProjectileScale
    );

    // 将世界坐标转换为屏幕坐标
    Vec2D targetScreenPos, predictedScreenPos;
    if (!gameView->WorldToScreen(targetBonePos, targetScreenPos) ||
        !gameView->WorldToScreen(predictedPos, predictedScreenPos)) {
        return;
    }

    // 绘制预测线和点
    auto drawList = ImGui::GetBackgroundDrawList();

    // 当前位置标记
    drawList->AddCircleFilled(
        ImVec2(targetScreenPos.x, targetScreenPos.y),
        5.0f,
        ImGui::ColorConvertFloat4ToU32(ImVec4(1.0f, 0.5f, 0.0f, 0.8f))
    );

    // 预测位置标记
    drawList->AddCircleFilled(
        ImVec2(predictedScreenPos.x, predictedScreenPos.y),
        5.0f,
        ImGui::ColorConvertFloat4ToU32(ImVec4(0.0f, 1.0f, 0.0f, 0.8f))
    );

    // 预测路径线
    drawList->AddLine(
        ImVec2(targetScreenPos.x, targetScreenPos.y),
        ImVec2(predictedScreenPos.x, predictedScreenPos.y),
        ImGui::ColorConvertFloat4ToU32(ImVec4(1.0f, 1.0f, 0.0f, 0.8f)),
        2.0f
    );
}

void Aimbot::renderUI() {
    // 标题和分隔线
    ImGui::Text("%s", t_("AIMBOT SETTINGS"));
    ImGui::Separator();
    ImGui::Spacing();

    // 添加顶部水平导航 - 简化版本
    static int aimbotTab = 0; // 0=Global, 1=Settings, 2=Hitboxes

    // 创建简洁的标签
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(16, 8));
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.18f, 0.18f, 0.18f, 0.8f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.25f, 0.25f, 0.25f, 0.9f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.20f, 0.30f, 0.40f, 1.0f));

    // 水平排列标签
    if (ImGui::Button(t_("Global"), ImVec2(100, 36))) aimbotTab = 0;
    ImGui::SameLine();
    if (ImGui::Button(t_("Settings"), ImVec2(100, 36))) aimbotTab = 1;
    ImGui::SameLine();
    if (ImGui::Button(t_("Hitboxes"), ImVec2(100, 36))) aimbotTab = 2;
    ImGui::SameLine();
    if (ImGui::Button(t_("Prediction"), ImVec2(100, 36))) aimbotTab = 3;
    // SkyNade button removed - functionality disabled

    ImGui::PopStyleColor(3);
    ImGui::PopStyleVar();

    ImGui::Spacing();
    ImGui::Spacing();

    // 根据选择的标签页显示不同内容
    switch (aimbotTab) {
    case 0: // Global
        renderGlobalTabSimplified();
        break;
    case 1: // Settings
        renderSettingsTabSimplified();
        break;
    case 2: // Hitboxes
        renderHitboxesTabSimplified();
        break;
    case 3: // 预测设置
        renderPredictionSettings();
        break;
    }
}

void Aimbot::renderSkyNadeSettings() {
    ImGui::BeginChild("SkyNadeSettings", ImVec2(0, 0), false);

    // SkyNade functionality completely removed
    ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.5f, 1.0f), "SkyNade functionality has been disabled");

    ImGui::EndChild();
}

void Aimbot::renderGlobalTabSimplified() {
    ImGui::BeginChild("GlobalContent", ImVec2(0, 0), false);

    // 左右两列布局
    float width = ImGui::GetContentRegionAvail().x;
    float leftWidth = width * 0.5f - 10;
    float rightWidth = width * 0.5f - 10;

    // 左侧面板
    ImGui::BeginChild("LeftPanel", ImVec2(leftWidth, 0), false);

    // 主开关 - 大号开关
    {
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8, 8));
        ImGui::PushStyleColor(ImGuiCol_CheckMark, ImVec4(0.3f, 0.7f, 0.3f, 1.0f));
        ImGui::Checkbox(t_("Enable Aimbot"), &aimParams.enabled);
        ImGui::PopStyleColor();
        ImGui::PopStyleVar();
    }

    ImGui::Spacing();
    ImGui::Spacing();

    // 基本设置组
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8f, 0.9f, 1.0f, 1.0f));
    ImGui::Text("%s", t_("Basic Settings"));
    ImGui::PopStyleColor();
    ImGui::Separator();

    // Only KmBox method available
    ImGui::Text("%s", t_("Aim Type"));
    ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "%s", t_("KmBox (Recommended)"));

    ImGui::Spacing();

    ImGui::PushID("Normal");
    // FOV和平滑度设置 - 简化版
    ImGui::Text("%s", t_("Normal Mode"));
    ImGui::PushItemWidth(leftWidth * 0.8f);
    ImGui::SliderFloat(t_("FOV##Normal"), &aimParams.noScope.fov, 1.0f, 180.0f, "%.1f");
    ImGui::SliderFloat(t_("Smooth##Normal"), &aimParams.noScope.smooth, 1.0f, 100.0f, "%.1f");
    ImGui::PopItemWidth();
	ImGui::PopID();
    ImGui::Spacing();

	ImGui::PushID("Scoped");
    ImGui::Text("%s", t_("Scoped Mode"));
    ImGui::PushItemWidth(leftWidth * 0.8f);
    ImGui::SliderFloat(t_("FOV##Scoped"), &aimParams.scoped.fov, 1.0f, 180.0f, "%.1f");
    ImGui::SliderFloat(t_("Smooth##Scoped"), &aimParams.scoped.smooth, 1.0f, 100.0f, "%.1f");
    ImGui::PopItemWidth();
	ImGui::PopID();
    ImGui::Spacing();

    kmbox->renderKmboxSettings();

    ImGui::EndChild();

    ImGui::SameLine();

    // 右侧面板
    ImGui::BeginChild("RightPanel", ImVec2(rightWidth, 0), false);

    // 按键绑定组
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8f, 0.9f, 1.0f, 1.0f));
    ImGui::Text("%s", t_("Key Bindings"));
    ImGui::PopStyleColor();
    ImGui::Separator();

    ImGui::Spacing();

    auto& renderer = ImGuiRenderer::getInstance();

    if (!misc->uiParams.useController)
    {
        renderer.renderKeyBinding(t_("Primary Aim"), &keys.primary);
        // Secondary Aim and Force Head Aim buttons removed
    }
    else
    {
        renderer.renderGamepadBinding(t_("Primary Aim"), &keys.primaryController);
        // Secondary Aim and Force Head Aim buttons removed
    }
    

    ImGui::Spacing();
    ImGui::Spacing();

    // 视觉设置组
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8f, 0.9f, 1.0f, 1.0f));
    ImGui::Text("%s", t_("Visual Settings"));
    ImGui::PopStyleColor();
    ImGui::Separator();

    ImGui::Spacing();

    // FOV圆圈设置
    ImGui::Checkbox(t_("Show FOV Circle"), &aimParams.showFOV);

    if (aimParams.showFOV) {
        ImGui::PushItemWidth(rightWidth * 0.8f);
        ImGui::SliderFloat(t_("FOV Circle Thickness"), &aimParams.fovThickness, 0.5f, 5.0f, "%.1f");
        ImGui::PopItemWidth();

        // FOV圆圈颜色
        ImGui::Text("%s:", t_("Normal Color"));
        ImGui::SameLine();
        ImGui::ColorEdit4("##NormalColor", (float*)&visualSettings.normalColor,
            ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar);

        ImGui::Text("%s:", t_("Locked Color"));
        ImGui::SameLine();
        ImGui::ColorEdit4("##TargetColor", (float*)&visualSettings.targetColor,
            ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar);
    }

    ImGui::EndChild();

    ImGui::EndChild(); // GlobalContent
}



// 简化版Settings标签页
void Aimbot::renderSettingsTabSimplified() {
    ImGui::BeginChild("SettingsContent", ImVec2(0, 0), false);

    // 左右两列布局
    float width = ImGui::GetContentRegionAvail().x;
    float leftWidth = width * 0.5f - 10;
    float rightWidth = width * 0.5f - 10;

    // 左侧面板
    ImGui::BeginChild("LeftPanel", ImVec2(leftWidth, 0), false);

    // 目标选择设置
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8f, 0.9f, 1.0f, 1.0f));
    ImGui::Text("%s", t_("Target Selection"));
    ImGui::PopStyleColor();
    ImGui::Separator();

    ImGui::Spacing();

    // 目标类型选择 - 更新为使用实际变量
    ImGui::Text("%s", t_("Target Types"));
    ImGui::Checkbox(t_("Target Enemies"), &targetSettings.targetEnemies);
    ImGui::Checkbox(t_("Target Teammates"), &targetSettings.targetTeammates);
    ImGui::Checkbox(t_("Target Bots"), &targetSettings.targetBots);

    ImGui::Spacing();

    // 目标优先级 - 更新为使用实际变量
    ImGui::Text("%s", t_("Priority Mode"));
    const char* priorities[] = { t_("Distance"), t_("FOV"), t_("Health") };
    ImGui::PushItemWidth(leftWidth * 0.8f);
    ImGui::Combo("##Priority", &targetSettings.priorityMode, priorities, IM_ARRAYSIZE(priorities));
    ImGui::PopItemWidth();

    ImGui::Spacing();

    ImGui::Text("%s", t_("Target Filtering"));
    ImGui::Checkbox(t_("Visible Only"), &targetSettings.visibleOnly);
    ImGui::Checkbox(t_("Ignore Knocked Down"), &targetSettings.ignoreKnockedDown);

    ImGui::Spacing();
    ImGui::Spacing();

    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8f, 0.9f, 1.0f, 1.0f));
    ImGui::Text("%s", t_("Humanization Settings"));
    ImGui::PopStyleColor();
    ImGui::Separator();

    ImGui::Spacing();

    ImGui::Checkbox(t_("Enable Humanization"), &humanParams.enabled);

    if (humanParams.enabled) {
        ImGui::PushItemWidth(leftWidth * 0.5f);

        // 添加人性化模式选择
        const char* humanizeModes[] = { t_("Curve Offset"), t_("Bezier Path") };
        ImGui::Combo(t_("Humanize Mode"), &humanParams.humanizeMode, humanizeModes, IM_ARRAYSIZE(humanizeModes));

        // 根据选择的模式显示不同的设置
        if (humanParams.humanizeMode == 0) {
            // 模式1设置
            ImGui::DragFloatRange2(t_("Curve Offset Range"), &humanParams.curveOffsetMin, &humanParams.curveOffsetMax,
                10.f, 20.f, 50.f, "Min: %.1f", "Max: %.1f");
        }
        else {
            // 模式2设置(原有设置)
            const char* types[] = { t_("All"), t_("Only on aimed at"), t_("Only on shoot") };
            ImGui::Combo(t_("Adjust Type"), &humanParams.type, types, IM_ARRAYSIZE(types));
            ImGui::SliderFloat(t_("Micro-Adjustment Scale"), &humanParams.microAdjustScale, 0.0f, 0.3f, "%.03f");

            // 抖动方向选择
            ImGui::Text("%s:", t_("Adjustment Directions"));
            ImGui::Checkbox(t_("Horizontal (Yaw)"), &humanParams.enableYawAdjust);
            ImGui::Checkbox(t_("Vertical (Pitch)"), &humanParams.enablePitchAdjust);

            if (!humanParams.enableYawAdjust && !humanParams.enablePitchAdjust) {
                ImGui::TextColored(ImVec4(1.0f, 0.3f, 0.3f, 1.0f), "%s", t_("Warning: All directions disabled!"));
            }

            ImGui::SliderFloat(t_("Non-Linear Path Strength"), &humanParams.nonLinearPathStrength, 0.0f, 1.0f, "%.2f");
            ImGui::DragFloatRange2(t_("Time Variation"), &humanParams.timeVariationMin, &humanParams.timeVariationMax,
                0.01f, 0.5f, 2.0f, "Min: %.2f", "Max: %.2f");
        }

        ImGui::PopItemWidth();
    }

    ImGui::Checkbox(t_("Show Lock Indicator"), &humanParams.showLockIndicator);
    if (humanParams.showLockIndicator) {
        ImGui::PushItemWidth(leftWidth * 0.8f);
        ImGui::SliderFloat(t_("Indicator Size"), &humanParams.indicatorSize, 3.0f, 15.0f, "%.1f");
        ImGui::ColorEdit4(t_("Indicator Color"), (float*)&humanParams.indicatorColor,
            ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar);
        ImGui::PopItemWidth();
    }

    ImGui::EndChild();

    ImGui::SameLine();

    // 右侧面板
    ImGui::BeginChild("RightPanel", ImVec2(rightWidth, 0), false);

    // 高级设置
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8f, 0.9f, 1.0f, 1.0f));
    ImGui::Text("%s", t_("Advanced Settings"));
    ImGui::PopStyleColor();
    ImGui::Separator();

    ImGui::Spacing();

    // 后坐力补偿
    ImGui::Text("%s", t_("Recoil Compensation"));

    ImGui::PushItemWidth(rightWidth * 0.8f);
    ImGui::Text("%s:", t_("Vertical"));
    ImGui::SliderFloat("##VerticalRecoil", &aimParams.noScope.recoilX, 0.0f, 2.0f, "%.2fx");

    ImGui::Text("%s:", t_("Horizontal"));
    ImGui::SliderFloat("##HorizontalRecoil", &aimParams.noScope.recoilY, 0.0f, 2.0f, "%.2fx");

    ImGui::Text("%s:", t_("Scoped Vertical"));
    ImGui::SliderFloat("##ScopedVerticalRecoil", &aimParams.scoped.recoilX, 0.0f, 2.0f, "%.2fx");

    ImGui::Text("%s:", t_("Scoped Horizontal"));
    ImGui::SliderFloat("##ScopedHorizontalRecoil", &aimParams.scoped.recoilY, 0.0f, 2.0f, "%.2fx");
    ImGui::PopItemWidth();

    ImGui::Spacing();
    ImGui::Spacing();

    // 平滑度设置
    ImGui::Text("%s", t_("Smoothing Settings"));

    ImGui::Text("%s: ", t_("SwitchSmoothScale"));
    ImGui::PushItemWidth(rightWidth * 0.8f);
    ImGui::SliderFloat("#SwitchSmooth", &aimParams.switchSmoothScale, 1.0f, 10.0f, "%.1fx");
    ImGui::Text("%s: ", t_("ShootingSmooth"));
    ImGui::SliderFloat("#ShootingSmooth", &aimParams.shootingSmoothScale, 0.1f, 2.0f, "%.2fx");
    ImGui::PopItemWidth();

    ImGui::EndChild();

    ImGui::EndChild(); // SettingsContent
}

// 简化版Hitboxes标签页
void Aimbot::renderHitboxesTabSimplified() {
    ImGui::BeginChild("HitboxesContent", ImVec2(0, 0), false);

    // 左右两列布局
    float width = ImGui::GetContentRegionAvail().x;
    float leftWidth = width * 0.4f - 10;
    float rightWidth = width * 0.6f - 10;

    // 左侧面板
    ImGui::BeginChild("LeftPanel", ImVec2(leftWidth, 0), false);

    // 骨骼选择设置
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8f, 0.9f, 1.0f, 1.0f));
    ImGui::Text("%s", t_("Hitbox Selection"));
    ImGui::PopStyleColor();
    ImGui::Separator();

    ImGui::Spacing();

    // 自动选择骨骼开关
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(6, 6));
    ImGui::Checkbox(t_("Shield Check"), &aimParams.shieldCheck);
    ImGui::Checkbox(t_("Auto Select Best Bone"), &aimParams.autoAimBone);
    ImGui::PopStyleVar();

    ImGui::Spacing();

    if (!aimParams.autoAimBone) {
        // 骨骼选择下拉框
        ImGui::Text("%s", t_("Target Bone"));
        const char* bones[] = { t_("Head"), t_("Neck"), t_("Upper Chest"), t_("Lower Chest"), t_("Stomach"), t_("Pelvis") };
        ImGui::PushItemWidth(leftWidth * 0.9f);
        ImGui::Combo("##TargetBone", &aimParams.aimBone, bones, IM_ARRAYSIZE(bones));
        ImGui::PopItemWidth();
    }

    ImGui::Spacing();
    ImGui::Spacing();

    // 目标特定设置 - 更新为使用实际变量
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8f, 0.9f, 1.0f, 1.0f));
    ImGui::Text("%s", t_("Target Specific Settings"));
    ImGui::PopStyleColor();
    ImGui::Separator();

    ImGui::Spacing();

    // 针对不同目标类型的设置
    ImGui::Text("%s", t_("Target Type"));
    const char* targetTypes[] = { t_("All Targets"), t_("No Shield Players"), t_("Shield Players") };
    ImGui::PushItemWidth(leftWidth * 0.9f);
    ImGui::Combo("##TargetType", &targetTypeSettings.currentTargetType, targetTypes, IM_ARRAYSIZE(targetTypes));
    ImGui::PopItemWidth();

    ImGui::Spacing();

    // 根据选择的目标类型显示不同的骨骼设置
    ImGui::Text("%s:", t_("Default Bone for"));
    ImGui::SameLine();
    ImGui::Text("%s", t_(targetTypes[targetTypeSettings.currentTargetType]));
    const char* bones[] = { t_("Head"), t_("Neck"), t_("Upper Chest"), t_("Lower Chest"), t_("Stomach"), t_("Pelvis") };
    ImGui::PushItemWidth(leftWidth * 0.9f);
    ImGui::Combo("##DefaultBone", &targetTypeSettings.defaultBones[targetTypeSettings.currentTargetType], bones, IM_ARRAYSIZE(bones));
    ImGui::PopItemWidth();

    ImGui::EndChild();

    ImGui::SameLine();

    // 右侧面板 - 骨骼可视化
    ImGui::BeginChild("RightPanel", ImVec2(rightWidth, 0), false);

    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8f, 0.9f, 1.0f, 1.0f));
    ImGui::Text("%s", t_("Hitbox Visualization"));
    ImGui::PopStyleColor();
    ImGui::Separator();

    ImGui::Spacing();

    // 创建一个更大的区域来绘制骨骼图
    ImVec2 canvasPos = ImGui::GetCursorScreenPos();
    ImVec2 canvasSize = ImVec2(ImGui::GetContentRegionAvail().x, 400);
    ImGui::InvisibleButton("canvas", canvasSize);

    // 绘制骨骼图示
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    float centerX = canvasPos.x + canvasSize.x / 2;
    float centerY = canvasPos.y + canvasSize.y / 2 - 50;

    // 定义骨骼位置
    float scale = 2.0f; // 缩放因子，使图更大

    // 头部
    float headRadius = 20 * scale;
    ImVec2 headPos(centerX, centerY - 60 * scale);
    bool headSelected = aimParams.aimBone == 0 || aimParams.autoAimBone;
    drawList->AddCircle(headPos, headRadius,
        headSelected ? IM_COL32(255, 0, 0, 255) : IM_COL32(255, 255, 255, 255),
        0, 2.0f * scale);
    drawList->AddText(ImVec2(headPos.x + headRadius + 5, headPos.y - 10),
        IM_COL32(255, 255, 255, 255), t_("Head"));

    // 颈部
    ImVec2 neckTop(centerX, centerY - 40 * scale);
    ImVec2 neckBottom(centerX, centerY - 30 * scale);
    bool neckSelected = aimParams.aimBone == 1 || aimParams.autoAimBone;
    drawList->AddLine(neckTop, neckBottom,
        neckSelected ? IM_COL32(255, 0, 0, 255) : IM_COL32(255, 255, 255, 255),
        2.0f * scale);
    drawList->AddText(ImVec2(neckBottom.x + 30, neckBottom.y - 5),
        IM_COL32(255, 255, 255, 255), t_("Neck"));

    // 上胸
    ImVec2 upperChestTop = neckBottom;
    ImVec2 upperChestBottom(centerX, centerY);
    ImVec2 upperChestLeft(centerX - 20 * scale, centerY - 20 * scale);
    ImVec2 upperChestRight(centerX + 20 * scale, centerY - 20 * scale);
    bool upperChestSelected = aimParams.aimBone == 2 || aimParams.autoAimBone;

    drawList->AddQuad(
        upperChestTop, upperChestRight, upperChestBottom, upperChestLeft,
        upperChestSelected ? IM_COL32(255, 0, 0, 150) : IM_COL32(255, 255, 255, 150),
        2.0f * scale
    );
    drawList->AddText(ImVec2(upperChestRight.x + 5, upperChestRight.y),
        IM_COL32(255, 255, 255, 255), t_("Upper Chest"));

    // 下胸
    ImVec2 lowerChestTop = upperChestBottom;
    ImVec2 lowerChestBottom(centerX, centerY + 20 * scale);
    ImVec2 lowerChestLeft(centerX - 22 * scale, centerY + 10 * scale);
    ImVec2 lowerChestRight(centerX + 22 * scale, centerY + 10 * scale);
    bool lowerChestSelected = aimParams.aimBone == 3 || aimParams.autoAimBone;

    drawList->AddQuad(
        lowerChestTop, lowerChestRight, lowerChestBottom, lowerChestLeft,
        lowerChestSelected ? IM_COL32(255, 0, 0, 150) : IM_COL32(255, 255, 255, 150),
        2.0f * scale
    );
    drawList->AddText(ImVec2(lowerChestRight.x + 5, lowerChestRight.y),
        IM_COL32(255, 255, 255, 255), t_("Lower Chest"));

    // 腹部
    ImVec2 stomachTop = lowerChestBottom;
    ImVec2 stomachBottom(centerX, centerY + 40 * scale);
    ImVec2 stomachLeft(centerX - 18 * scale, centerY + 30 * scale);
    ImVec2 stomachRight(centerX + 18 * scale, centerY + 30 * scale);
    bool stomachSelected = aimParams.aimBone == 4 || aimParams.autoAimBone;

    drawList->AddQuad(
        stomachTop, stomachRight, stomachBottom, stomachLeft,
        stomachSelected ? IM_COL32(255, 0, 0, 150) : IM_COL32(255, 255, 255, 150),
        2.0f * scale
    );
    drawList->AddText(ImVec2(stomachRight.x + 5, stomachRight.y),
        IM_COL32(255, 255, 255, 255), t_("Stomach"));

    // 骨盆
    ImVec2 pelvisTop = stomachBottom;
    ImVec2 pelvisBottom(centerX, centerY + 60 * scale);
    ImVec2 pelvisLeft(centerX - 25 * scale, centerY + 50 * scale);
    ImVec2 pelvisRight(centerX + 25 * scale, centerY + 50 * scale);
    bool pelvisSelected = aimParams.aimBone == 5 || aimParams.autoAimBone;

    drawList->AddQuad(
        pelvisTop, pelvisRight, pelvisBottom, pelvisLeft,
        pelvisSelected ? IM_COL32(255, 0, 0, 150) : IM_COL32(255, 255, 255, 150),
        2.0f * scale
    );
    drawList->AddText(ImVec2(pelvisRight.x + 5, pelvisRight.y),
        IM_COL32(255, 255, 255, 255), t_("Pelvis"));

    // 添加射击轨迹可视化
    if (aimParams.autoAimBone) {
        drawList->AddBezierCubic(
            ImVec2(centerX - 100, centerY + 150),  // 起点
            ImVec2(centerX - 50, centerY + 50),    // 控制点1
            ImVec2(centerX + 50, centerY - 50),    // 控制点2
            headPos,                               // 终点 (头部)
            IM_COL32(255, 255, 0, 200),            // 黄色
            2.0f * scale,                          // 线条粗细
            20                                     // 段数
        );
    }
    else {
        // 根据选择的骨骼确定终点
        ImVec2 targetPos;
        switch (aimParams.aimBone) {
        case 0: targetPos = headPos; break;
        case 1: targetPos = neckBottom; break;
        case 2: targetPos = upperChestBottom; break;
        case 3: targetPos = lowerChestBottom; break;
        case 4: targetPos = stomachBottom; break;
        case 5: targetPos = pelvisBottom; break;
        default: targetPos = headPos; break;
        }

        drawList->AddBezierCubic(
            ImVec2(centerX - 100, centerY + 150),  // 起点
            ImVec2(centerX - 50, centerY + 50),    // 控制点1
            ImVec2(centerX + 50, centerY),         // 控制点2
            targetPos,                             // 终点 (选中的骨骼)
            IM_COL32(255, 255, 0, 200),            // 黄色
            2.0f * scale,                          // 线条粗细
            20                                     // 段数
        );
    }

    ImGui::EndChild();

    ImGui::EndChild(); // HitboxesContent
}

void Aimbot::renderPredictionSettings() {
    ImGui::BeginChild("PredictionSettings", ImVec2(0, 0), false);

    // 标题和主开关
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8f, 0.9f, 1.0f, 1.0f));
    ImGui::Text("%s", t_("Prediction System Settings"));
    ImGui::PopStyleColor();
    ImGui::Separator();
    ImGui::Spacing();

    ImGui::Checkbox(t_("Enable Prediction"), &predictionSettings.enabled);
    triggerbot->HelpMarker(t_("Predicts target movement for projectile weapons"));

    if (predictionSettings.enabled) {
        // 预测系统选择 - только AimCorrection Method
        ImGui::Text("%s", t_("Prediction System"));
        ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x * 0.8f);
        ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "%s", t_("Prediction Method: AimCorrection"));
        ImGui::PopItemWidth();

        // 系统描述
        ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "%s", t_("AimCorrection Method: Simple and fast"));

        ImGui::Spacing();

        // 通用设置
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8f, 0.9f, 1.0f, 1.0f));
        ImGui::Text("%s", t_("General Settings"));
        ImGui::PopStyleColor();
        ImGui::Separator();

        // 预测比例
        ImGui::Text("%s", t_("PredictionScale"));
        ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x * 0.8f);
        ImGui::SliderFloat("##PredScale", &predictionSettings.scale, 0.1f, 2.0f, "%.2fx");
        ImGui::PopItemWidth();

        ImGui::Checkbox(t_("Show Prediction Point"), &predictionSettings.showDebug);
    }

    ImGui::EndChild();
}
