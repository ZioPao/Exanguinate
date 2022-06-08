modded class SCR_CharacterDamageManagerComponent : ScriptedDamageManagerComponent
{

	[Attribute("{9DEEB4290C38BF09}Particles/Enviroment/BleedingBodyShots.ptc", UIWidgets.Auto, desc: "Bleeding particle effect from body shots", category: "Bleeding", params: "ptc")]
	private ResourceName m_sBleedingParticleBody;
	
	[Attribute("{486BD20FE5257A35}Particles/Enviroment/BleedingHeadshots.ptc", UIWidgets.Auto, desc: "Bleeding particle effect from head shots", category: "Bleeding", params: "ptc")]
	private ResourceName m_sBleedingParticleHeadshot;
	

	bool enableEffects;

	
	ref map<string, string> exaSettings;
	

	override void OnInit(IEntity owner)
	{
		super.OnInit(owner);
		
		enableEffects = true;
		

	}
	
	
	
	
	void InitBloodlust(IEntity owner)
	{
		auto world = owner.GetWorld();
	
		m_splatterBehavior = new BL_CharacterSplatterBehavior;
		m_splatterBehavior.OnInit(owner, world, this);
	
		m_bleedBehavior = new BL_CharacterBleedBehavior;
		m_bleedBehavior.OnInit(owner, world, this);
	}
	
	

//-----------------------------------------------------------------------------------------------------------
	//! Called to start bleeding effect on a specificied bone 		
	// THIS DOESN'T NEED THE SUPER SINCE WE'RE REPLACING IT ENTIRELY!!!
	override void CreateBleedingParticleEffect(notnull HitZone hitZone, float bleedingRate, int colliderDescriptorIndex)
	{
		
		
		/////////////////////////////////////////////////////////////////////////////////////////////////
		//Settings initialization stuff 
		MCF_SettingsManager EXA_mcfSettingsManager = MCF_SettingsManager.GetInstance();
		const string EXA_FileNameJson = "EXA_Settings.json";
		const string EXA_MOD_ID = "597CFDF094D52251";				//it's probably possible to get this in a better way but ok
		map<string, ref VariableInfo> exaVariableMap = new map<string, ref VariableInfo>();
		
		exaVariableMap.Set("enableEffects", new VariableInfo("Enable Effects", "1"));
		
		
		// still janky
		if (!EXA_mcfSettingsManager.GetJsonManager(EXA_MOD_ID))
		{
			Print("EXA: Preparing MCF");
			exaSettings = EXA_mcfSettingsManager.Setup(EXA_MOD_ID, EXA_FileNameJson, exaVariableMap);
		
		}
		else if (!exaSettings)
		{
			Print("EXA: Loading Settings");

			exaSettings = EXA_mcfSettingsManager.GetModSettings(EXA_MOD_ID);
			
		}
		
		enableEffects = exaSettings.Get("enableEffects").ToInt();

		//////////////////////////////////////////////////////////////////////////////////////////////////
		
		
		
		if (enableEffects)
		{

			
			RemoveBleedingParticleEffect(hitZone);
			
			if (bleedingRate == 0 || m_fBleedingParticleRateScale == 0)
				return;
			
	
			ResourceName chosenParticles;
			
			
			// TODO: Blood traces on ground that should be left regardless of clothing, perhaps just delayed
			
			// Headshots are considered as NOT COVERED!
			SCR_CharacterHitZone characterHitZone = SCR_CharacterHitZone.Cast(hitZone);
			
			

			if (characterHitZone.IsCovered())
				chosenParticles = m_sBleedingParticleBody;
			else
			{
				chosenParticles = m_sBleedingParticleHeadshot;

			}
			
			
			// Get bone node
			vector transform[4];
			int boneIndex;
			int boneNode;
			if (!hitZone.TryGetColliderDescription(GetOwner(), colliderDescriptorIndex, transform, boneIndex, boneNode))
				return;
			
			SCR_ParticleEmitter particleEmitter = SCR_ParticleAPI.PlayOnObjectPTC(GetOwner(), chosenParticles, vector.Zero, vector.Zero, boneNode);
			SCR_ParticleAPI.LerpAllEmitters(particleEmitter, bleedingRate * m_fBleedingParticleRateScale, EmitterParam.BIRTH_RATE);
			
			if (!m_mBleedingParticles)
				m_mBleedingParticles = new map<HitZone, SCR_ParticleEmitter>;
			
			m_mBleedingParticles.Insert(hitZone, particleEmitter);
			
		}
		
		

	}
	
	
	
};
