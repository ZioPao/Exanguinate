modded class SCR_CharacterDamageManagerComponent : ScriptedDamageManagerComponent
{

	[Attribute("{9DEEB4290C38BF09}Particles/Enviroment/AddedBleeding.ptc", UIWidgets.Auto, desc: "Bleeding particle effect from body shots", category: "Bleeding", params: "ptc")]
	private ResourceName m_sBleedingParticleMini;

	bool enableEffects;
	
	override void OnInit(IEntity owner)
	{

		/* Settings initialization stuff */
		MCF_SettingsManager mcfSettingsManager = MCF_SettingsManager.GetInstance();
		
		const string fileNameJson = "EXA_Settings.json";
		const string MOD_ID = "597CFDF094D52251";				//it's probably possible to get this in a better way but ok
		map<string, string> mapDefaultValues = new map<string, string>();
		mapDefaultValues.Insert("enableEffects", "true");
		array<string> userFriendlyNames = {"Enable Effects"};

		map<string, string> settings = mcfSettingsManager.Setup(MOD_ID, fileNameJson, mapDefaultValues, userFriendlyNames);
		enableEffects = settings.Get("enableEffects").ToInt();

	}

//-----------------------------------------------------------------------------------------------------------
	//! Called to start bleeding effect on a specificied bone
	override void CreateBleedingParticleEffect(notnull HitZone hitZone, float bleedingRate, int colliderDescriptorIndex)
	{
		
		if (enableEffects)
		{
			// Play Bleeding particle
			if (m_sBleedingParticle.IsEmpty())
				return;
			
			RemoveBleedingParticleEffect(hitZone);
			
			if (bleedingRate == 0 || m_fBleedingParticleRateScale == 0)
				return;
			
	
			ResourceName chosenParticles;
			
			
			// TODO: Blood traces on ground that should be left regardless of clothing, perhaps just delayed
			SCR_CharacterHitZone characterHitZone = SCR_CharacterHitZone.Cast(hitZone);
			if (characterHitZone.IsCovered())
				chosenParticles = m_sBleedingParticleMini;
			else
				chosenParticles = m_sBleedingParticle;
			
			
			// Get bone node
			vector transform[4];
			int boneIndex;
			int boneNode;
			if (!hitZone.TryGetColliderDescription(GetOwner(), colliderDescriptorIndex, transform, boneIndex, boneNode))
				return;
			
			SCR_ParticleEmitter particleEmitter = SCR_ParticleAPI.PlayOnObjectPTC(GetOwner(), m_sBleedingParticle, vector.Zero, vector.Zero, boneNode);
			SCR_ParticleAPI.LerpAllEmitters(particleEmitter, bleedingRate * m_fBleedingParticleRateScale, EmitterParam.BIRTH_RATE);
			
			if (!m_mBleedingParticles)
				m_mBleedingParticles = new map<HitZone, SCR_ParticleEmitter>;
			
			m_mBleedingParticles.Insert(hitZone, particleEmitter);
			
		}
		
		

	}
	
	
	
};
