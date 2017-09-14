// <copyright file="PresentationFormats.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//----------------------------------------------------------------------------------------------

namespace SDKTemplate
{
    /// <summary>
    /// Class containing the details of Gatt characteristics presentation formats
    /// </summary>
    public class PresentationFormats
    {
        /// <summary>
        /// Units are established international standards for the measurement of physical quantities.
        /// </summary>
        /// <remarks>Please refer https://www.bluetooth.com/specifications/assigned-numbers/units </remarks>
        public enum Units
        {
            Unitless = 0x2700,
            LengthMetre = 0x2701,
            MassKilogram = 0x2702,
            TimeSecond = 0x2703,
            ElectricCurrentAmpere = 0x2704,
            ThermodynamicTemperatureKelvin = 0x2705,
            AmountOfSubstanceMole = 0x2706,
            LuminousIntensityCandela = 0x2707,
            AreaSquareMetres = 0x2710,
            VolumeCubicMetres = 0x2711,
            VelocityMetresPerSecond = 0x2712,
            AccelerationMetresPerSecondSquared = 0x2713,
            WaveNumberReciprocalMetre = 0x2714,
            DensityKilogramperCubicMetre = 0x2715,
            SurfaceDensityKilogramPerSquareMetre = 0x2716,
            SpecificVolumeCubicMetrePerKilogram = 0x2717,
            CurrentDensityAmperePerSquareMetre = 0x2718,
            MagneticFieldStrengthAmperePerMetre = 0x2719,
            AmountConcentrationMolePerCubicMetre = 0x271A,
            MassConcentrationKilogramPerCubicMetre = 0x271B,
            LuminanceCandelaPerSquareMetre = 0x271C,
            RefractiveIndex = 0x271D,
            RelativePermeability = 0x271E,
            PlaneAngleRadian = 0x2720,
            SolidAngleSteradian = 0x2721,
            FrequencyHertz = 0x2722,
            ForceNewton = 0x2723,
            PressurePascal = 0x2724,
            EnergyJoule = 0x2725,
            PowerWatt = 0x2726,
            ElectricChargeCoulomb = 0x2727,
            ElectricPotentialDifferenceVolt = 0x2728,
            CapacitanceFarad = 0x2729,
            ElectricResistanceOhm = 0x272A,
            ElectricConductanceSiemens = 0x272B,
            MagneticFluxWeber = 0x272C,
            MagneticFluxDensityTesla = 0x272D,
            InductanceHenry = 0x272E,
            CelsiusTemperatureDegreeCelsius = 0x272F,
            LuminousFluxLumen = 0x2730,
            IlluminanceLux = 0x2731,
            ActivityReferredToARadioNuclideBecquerel = 0x2732,
            AbsorbedDoseGray = 0x2733,
            DoseEquivalentSievert = 0x2734,
            CatalyticActivityKatal = 0x2735,
            DynamicViscosityPascalSecond = 0x2740,
            MomentOfForceNewtonMetre = 0x2741,
            SurfaceTensionNewtonPerMetre = 0x2742,
            AngularVelocityRadianPerSecond = 0x2743,
            AngularAccelerationRadianPerSecondSquared = 0x2744,
            HeatFluxDensityWattPerSquareMetre = 0x2745,
            HeatCapacityJoulePerKelvin = 0x2746,
            SpecificHeatCapacityJoulePerKilogramKelvin = 0x2747,
            SpecificEnergyJoulePerKilogram = 0x2748,
            ThermalConductivityWattPerMetreKelvin = 0x2749,
            EnergyDensityJoulePerCubicMetre = 0x274A,
            ElectricfieldstrengthVoltPerMetre = 0x274B,
            ElectricchargeDensityCoulombPerCubicMetre = 0x274C,
            SurfacechargeDensityCoulombPerSquareMetre = 0x274D,
            ElectricFluxDensityCoulombPerSquareMetre = 0x274E,
            PermittivityFaradPerMetre = 0x274F,
            PermeabilityHenryPerMetre = 0x2750,
            MolarEnergyJoulePermole = 0x2751,
            MolarentropyJoulePermoleKelvin = 0x2752,
            ExposureCoulombPerKilogram = 0x2753,
            AbsorbeddoserateGrayPerSecond = 0x2754,
            RadiantintensityWattPerSteradian = 0x2755,
            RadianceWattPerSquareMetreSteradian = 0x2756,
            CatalyticActivityConcentrationKatalPerCubicMetre = 0x2757,
            TimeMinute = 0x2760,
            TimeHour = 0x2761,
            TimeDay = 0x2762,
            PlaneAngleDegree = 0x2763,
            PlaneAngleMinute = 0x2764,
            PlaneAngleSecond = 0x2765,
            AreaHectare = 0x2766,
            VolumeLitre = 0x2767,
            MassTonne = 0x2768,
            PressureBar = 0x2780,
            PressureMilliMetreofmercury = 0x2781,
            LengthAngstrom = 0x2782,
            LengthNauticalMile = 0x2783,
            AreaBarn = 0x2784,
            VelocityKnot = 0x2785,
            LogarithmicRadioQuantityNeper = 0x2786,
            LogarithmicRadioQuantityBel = 0x2787,
            LengthYard = 0x27A0,
            LengthParsec = 0x27A1,
            LengthInch = 0x27A2,
            LengthFoot = 0x27A3,
            LengthMile = 0x27A4,
            PressurePoundForcePerSquareinch = 0x27A5,
            VelocityKiloMetrePerHour = 0x27A6,
            VelocityMilePerHour = 0x27A7,
            AngularVelocityRevolutionPerminute = 0x27A8,
            EnergyGramcalorie = 0x27A9,
            EnergyKilogramcalorie = 0x27AA,
            EnergyKiloWattHour = 0x27AB,
            ThermodynamicTemperatureDegreeFahrenheit = 0x27AC,
            Percentage = 0x27AD,
            PerMille = 0x27AE,
            PeriodBeatsPerMinute = 0x27AF,
            ElectricchargeAmpereHours = 0x27B0,
            MassDensityMilligramPerdeciLitre = 0x27B1,
            MassDensityMillimolePerLitre = 0x27B2,
            TimeYear = 0x27B3,
            TimeMonth = 0x27B4,
            ConcentrationCountPerCubicMetre = 0x27B5,
            IrradianceWattPerSquareMetre = 0x27B6,
            MilliliterPerKilogramPerminute = 0x27B7,
            MassPound = 0x27B8
        }

        /// <summary>
        /// The Name Space field is used to identify the organization that is responsible for defining the enumerations for the description field.
        /// </summary>
        /// <remarks>
        /// Please refer https://www.bluetooth.com/specifications/gatt/viewer?attributeXmlFile=org.bluetooth.descriptor.gatt.characteristic_presentation_format.xml
        /// </remarks>
        public enum NamespaceId
        {
            BluetoothSigAssignedNumber = 1,
            ReservedForFutureUse
        }

        /// <summary>
        /// The Description is an enumerated value from the organization identified by the Name Space field.
        /// </summary>
        /// <remarks>
        /// Please refer https://www.bluetooth.com/specifications/gatt/viewer?attributeXmlFile=org.bluetooth.descriptor.gatt.characteristic_presentation_format.xml
        /// </remarks>
        public const ushort Description = 0x0000;

        /// <summary>
        /// Exponent value for the characteristics
        /// </summary>
        public const int Exponent = 0;
    }
}
