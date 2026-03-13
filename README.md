

## 🔹 Corrected README.md

```markdown
# 🎮 C++ FPS Shooting Game (Unreal Engine)

## 📌 Objective
Develop a **First-Person Shooter (FPS)** game in **Unreal Engine (latest version)** using **native C++ APIs only**.  
The game dynamically spawns interactive boxes based on online JSON data. The player can shoot and damage these boxes, earning points upon destruction.

---

## 🌐 Online JSON Data
Boxes are spawned from JSON hosted at:  
[GitHub JSON Link](https://raw.githubusercontent.com/CyrusCHAU/Varadise-Technical-Test/refs/heads/main/data.json)

Each box entry includes:
- **Type** → For future expansions  
- **Color** → Applied dynamically to material  
- **Health** → Initial health value  
- **Score** → Points awarded upon destruction  

Error handling covers:
- HTTP request failure  
- JSON parse errors  
- Invalid/missing fields  

---

## 🟦 Box Spawning
- Boxes spawned via `SpawnActor` with collision handling.  
- Attributes applied using `ApplyBoxStats()` in C++.  
- Spawner injects **DestroySound** and **DestroyEffect** centrally.  
- Simple cube mesh used (UE default).  

---

## 🎨 Custom Material
- Material Instance Dynamic (MID) created with a `Base Color` parameter.  
- Color parsed from JSON and applied in C++:  
  ```cpp
  DynMat->SetVectorParameterValue("Base Color", BoxColor);
  ```

---

## 🔫 Shooting Mechanics
- Implemented using **line trace/projectile system**.  
- On hit:  
  - Box health reduced by 1  
  - If health ≤ 0 → Box destroyed  
  - Sound + particle effect triggered  
  - Score incremented via `ShooterGameMode`  

---

## 🖥️ Scoreboard UI
- Built with **UUserWidget** (C++ base + UMG extension).  
- Displays player’s current score.  
- Updates dynamically when a box is destroyed.  

---

## 📂 Project Structure
```
Source/
 └── Practce/
      ├── Public/
      │    ├── MyClass.h          // BoxActor class (health, score, color, effects)
      │    ├── BoxSpawner.h       // Spawner class (JSON fetch + spawn logic)
      │    ├── ShooterGameMode.h  // Game mode (score handling)
      │    └── ShooterHUD.h       // UI base class
      │
      ├── Private/
      │    ├── MyClass.cpp        // BoxActor implementation (damage, destroy, effects)
      │    ├── BoxSpawner.cpp     // JSON parsing + spawn logic
      │    ├── ShooterGameMode.cpp// Score increment logic
      │    └── ShooterHUD.cpp     // UI update logic
      │
      └── Practce.Build.cs        // Build configuration
```

---

## ✅ Requirements
- All gameplay logic in **C++ only**  
- UMG used only for UI (C++ base class required)  
- Default UE assets (cube mesh, projectile)  
- Smooth performance  
- Clean folder structure & documented code  

---

## 📌 Conclusion
This project demonstrates:
- **Data-driven gameplay** using online JSON  
- **C++ mastery in Unreal Engine**  
- **Cinematic polish** with sound + particle effects  
- **Recruiter-friendly documentation** with modular structure  

Copyright © 2022 Cyrus365, All Rights Reserved.


