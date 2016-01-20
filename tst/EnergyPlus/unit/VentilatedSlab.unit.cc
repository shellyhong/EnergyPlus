// EnergyPlus, Copyright (c) 1996-2016, The Board of Trustees of the University of Illinois and
// The Regents of the University of California, through Lawrence Berkeley National Laboratory
// (subject to receipt of any required approvals from the U.S. Dept. of Energy). All rights
// reserved.
//
// If you have questions about your rights to use or distribute this software, please contact
// Berkeley Lab's Innovation & Partnerships Office at IPO@lbl.gov.
//
// NOTICE: This Software was developed under funding from the U.S. Department of Energy and the
// U.S. Government consequently retains certain rights. As such, the U.S. Government has been
// granted for itself and others acting on its behalf a paid-up, nonexclusive, irrevocable,
// worldwide license in the Software to reproduce, distribute copies to the public, prepare
// derivative works, and perform publicly and display publicly, and to permit others to do so.
//
// Redistribution and use in source and binary forms, with or without modification, are permitted
// provided that the following conditions are met:
//
// (1) Redistributions of source code must retain the above copyright notice, this list of
//     conditions and the following disclaimer.
//
// (2) Redistributions in binary form must reproduce the above copyright notice, this list of
//     conditions and the following disclaimer in the documentation and/or other materials
//     provided with the distribution.
//
// (3) Neither the name of the University of California, Lawrence Berkeley National Laboratory,
//     the University of Illinois, U.S. Dept. of Energy nor the names of its contributors may be
//     used to endorse or promote products derived from this software without specific prior
//     written permission.
//
// (4) Use of EnergyPlus(TM) Name. If Licensee (i) distributes the software in stand-alone form
//     without changes from the version obtained under this License, or (ii) Licensee makes a
//     reference solely to the software portion of its product, Licensee must refer to the
//     software as "EnergyPlus version X" software, where "X" is the version number Licensee
//     obtained under this License and may not use a different name for the software. Except as
//     specifically required in this Section (4), Licensee shall not use in a company name, a
//     product name, in advertising, publicity, or other promotional activities any name, trade
//     name, trademark, logo, or other designation of "EnergyPlus", "E+", "e+" or confusingly
//     similar designation, without Lawrence Berkeley National Laboratory's prior written consent.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR
// IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
// AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
// OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//
// You are under no obligation whatsoever to provide any bug fixes, patches, or upgrades to the
// features, functionality or performance of the source code ("Enhancements") to anyone; however,
// if you choose to make your Enhancements available either publicly, or directly to Lawrence
// Berkeley National Laboratory, without imposing a separate written license agreement for such
// Enhancements, then you hereby grant the following license: a non-exclusive, royalty-free
// perpetual license to install, use, modify, prepare derivative works, incorporate into other
// computer software, distribute, and sublicense such enhancements or derivative works thereof,
// in binary and source code form.

// EnergyPlus::VentilatedSlab Unit Tests

// Google Test Headers
#include <gtest/gtest.h>

// EnergyPlus Headers
#include <EnergyPlus/UtilityRoutines.hh>
#include <EnergyPlus/VentilatedSlab.hh>
#include <EnergyPlus/DataHeatBalance.hh>
#include <EnergyPlus/DataLoopNode.hh>
#include <EnergyPlus/DataSurfaces.hh>
#include <EnergyPlus/DataSurfaceLists.hh>
#include <EnergyPlus/HeatBalanceManager.hh>
#include <EnergyPlus/ScheduleManager.hh>
#include <EnergyPlus/SurfaceGeometry.hh>


#include "Fixtures/EnergyPlusFixture.hh"

using namespace EnergyPlus;
using namespace ObjexxFCL;
using namespace DataGlobals;
using namespace EnergyPlus::VentilatedSlab;
using namespace EnergyPlus::DataHeatBalance;
using namespace EnergyPlus::DataLoopNode;
using namespace EnergyPlus::DataSurfaces;
using namespace EnergyPlus::DataSurfaceLists;
using namespace EnergyPlus::HeatBalanceManager;
using namespace EnergyPlus::ScheduleManager;
using namespace EnergyPlus::SurfaceGeometry;

TEST_F( EnergyPlusFixture, VentilatedSlab_CalcVentilatedSlabCoilOutputTest )
{

	BeginEnvrnFlag = false;
	Real64 PowerMet = 0.0;
	Real64 LatOutputProvided = 0.0;

	NumOfVentSlabs = 1;
	VentSlab.allocate( NumOfVentSlabs );
	int Item = 1;
	int FanOutletNode = 1;
	int OutletNode = 2;
	VentSlab( Item ).FanOutletNode = FanOutletNode;
	VentSlab( Item ).RadInNode = OutletNode;
	Node.allocate( 2 );
	Node( OutletNode ).MassFlowRate = 0.5;

	// Calcs being tested
	//	VentSlab( Item ).HeatCoilPower = max( 0.0, QUnitOut );
	//	VentSlab( Item ).SensCoolCoilPower = std::abs( min( 0.0, QUnitOut ) );
	//	VentSlab( Item ).TotCoolCoilPower = std::abs( min( 0.0, QTotUnitOut ) );
	//	VentSlab( Item ).LateCoolCoilPower = VentSlab( Item ).TotCoolCoilPower - VentSlab( Item ).SensCoolCoilPower;
	//	LatOutputProvided = AirMassFlow * ( SpecHumOut - SpecHumIn ); // Latent rate (kg/s), dehumid = negative
	//	PowerMet = QUnitOut;

	// Sensible Heating
	Node( FanOutletNode ).Temp = 15.0;
	Node( FanOutletNode ).HumRat = 0.003;
	Node( OutletNode ).Temp = 20.0;
	Node( OutletNode ).HumRat = 0.003;
	CalcVentilatedSlabCoilOutput( Item, PowerMet, LatOutputProvided );

	EXPECT_TRUE( VentSlab( Item ).HeatCoilPower > 0.0 );
	EXPECT_TRUE( VentSlab( Item ).SensCoolCoilPower == 0.0 );
	EXPECT_TRUE( VentSlab( Item ).TotCoolCoilPower == 0.0 );
	EXPECT_TRUE( VentSlab( Item ).LateCoolCoilPower == 0.0 );
	EXPECT_TRUE( LatOutputProvided == 0.0 );
	EXPECT_TRUE( PowerMet > 0.0 );

	// Sensible Cooling
	Node( FanOutletNode ).Temp = 25.0;
	Node( FanOutletNode ).HumRat = 0.003;
	Node( OutletNode ).Temp = 20.0;
	Node( OutletNode ).HumRat = 0.003;
	CalcVentilatedSlabCoilOutput( Item, PowerMet, LatOutputProvided );

	EXPECT_TRUE( VentSlab( Item ).HeatCoilPower == 0.0 );
	EXPECT_TRUE( VentSlab( Item ).SensCoolCoilPower > 0.0 );
	EXPECT_TRUE( VentSlab( Item ).TotCoolCoilPower == VentSlab( Item ).SensCoolCoilPower );
	EXPECT_TRUE( VentSlab( Item ).LateCoolCoilPower == 0.0 );
	EXPECT_TRUE( LatOutputProvided == 0.0 );
	EXPECT_TRUE( PowerMet < 0.0 );

	// Sensible and Latent Cooling
	Node( FanOutletNode ).Temp = 25.0;
	Node( FanOutletNode ).HumRat = 0.008;
	Node( OutletNode ).Temp = 20.0;
	Node( OutletNode ).HumRat = 0.003;
	CalcVentilatedSlabCoilOutput( Item, PowerMet, LatOutputProvided );

	EXPECT_TRUE( VentSlab( Item ).HeatCoilPower == 0.0 );
	EXPECT_TRUE( VentSlab( Item ).SensCoolCoilPower > 0.0 );
	EXPECT_TRUE( VentSlab( Item ).TotCoolCoilPower > VentSlab( Item ).SensCoolCoilPower );
	EXPECT_TRUE( VentSlab( Item ).LateCoolCoilPower > 0.0 );
	EXPECT_TRUE( LatOutputProvided < 0.0 );
	EXPECT_TRUE( PowerMet < 0.0 );

	// Deallocate everything
	VentSlab.deallocate();
	Node.deallocate();

}

TEST_F( EnergyPlusFixture, VentilatedSlab_InitVentilatedSlabTest ) {

	BeginEnvrnFlag = false;
	bool ErrorsFound( false );        // function returns true on error
	int Item( 1 ); // index for the current ventilated slab
	int VentSlabZoneNum( 1 ); // number of zone being served
	bool FirstHVACIteration( true ); // TRUE if 1st HVAC simulation of system timestep

	//Real64 PowerMet = 0.0;
	//Real64 LatOutputProvided = 0.0;

	//NumOfVentSlabs = 1;
	//VentSlab.allocate( NumOfVentSlabs );
	//int Item = 1;
	//int FanOutletNode = 1;
	//int OutletNode = 2;
	//VentSlab( Item ).FanOutletNode = FanOutletNode;
	//VentSlab( Item ).RadInNode = OutletNode;
	//Node.allocate( 2 );
	//Node( OutletNode ).MassFlowRate = 0.5;

	//// Sensible Heating
	//Node( FanOutletNode ).Temp = 15.0;
	//Node( FanOutletNode ).HumRat = 0.003;
	//Node( OutletNode ).Temp = 20.0;
	//Node( OutletNode ).HumRat = 0.003;
	//CalcVentilatedSlabCoilOutput( Item, PowerMet, LatOutputProvided );

	//EXPECT_TRUE( VentSlab( Item ).HeatCoilPower > 0.0 );
	//EXPECT_TRUE( VentSlab( Item ).SensCoolCoilPower == 0.0 );
	//EXPECT_TRUE( VentSlab( Item ).TotCoolCoilPower == 0.0 );
	//EXPECT_TRUE( VentSlab( Item ).LateCoolCoilPower == 0.0 );
	//EXPECT_TRUE( LatOutputProvided == 0.0 );
	//EXPECT_TRUE( PowerMet > 0.0 );

	//// Sensible Cooling
	//Node( FanOutletNode ).Temp = 25.0;
	//Node( FanOutletNode ).HumRat = 0.003;
	//Node( OutletNode ).Temp = 20.0;
	//Node( OutletNode ).HumRat = 0.003;
	//CalcVentilatedSlabCoilOutput( Item, PowerMet, LatOutputProvided );

	//EXPECT_TRUE( VentSlab( Item ).HeatCoilPower == 0.0 );
	//EXPECT_TRUE( VentSlab( Item ).SensCoolCoilPower > 0.0 );
	//EXPECT_TRUE( VentSlab( Item ).TotCoolCoilPower == VentSlab( Item ).SensCoolCoilPower );
	//EXPECT_TRUE( VentSlab( Item ).LateCoolCoilPower == 0.0 );
	//EXPECT_TRUE( LatOutputProvided == 0.0 );
	//EXPECT_TRUE( PowerMet < 0.0 );

	//// Sensible and Latent Cooling
	//Node( FanOutletNode ).Temp = 25.0;
	//Node( FanOutletNode ).HumRat = 0.008;
	//Node( OutletNode ).Temp = 20.0;
	//Node( OutletNode ).HumRat = 0.003;
	//CalcVentilatedSlabCoilOutput( Item, PowerMet, LatOutputProvided );

	//EXPECT_TRUE( VentSlab( Item ).HeatCoilPower == 0.0 );
	//EXPECT_TRUE( VentSlab( Item ).SensCoolCoilPower > 0.0 );
	//EXPECT_TRUE( VentSlab( Item ).TotCoolCoilPower > VentSlab( Item ).SensCoolCoilPower );
	//EXPECT_TRUE( VentSlab( Item ).LateCoolCoilPower > 0.0 );
	//EXPECT_TRUE( LatOutputProvided < 0.0 );
	//EXPECT_TRUE( PowerMet < 0.0 );

	std::string const idf_objects = delimited_string( {
	"  Version,8.4;",

		"  SimulationControl,",
		"    No,                      !- Do Zone Sizing Calculation",
		"    No,                      !- Do System Sizing Calculation",
		"    No,                      !- Do Plant Sizing Calculation",
		"    Yes,                     !- Run Simulation for Sizing Periods",
		"    No;                      !- Run Simulation for Weather File Run Periods",

		"  Building,",
		"    Building,                !- Name",
		"    30.,                     !- North Axis {deg}",
		"    City,                    !- Terrain",
		"    0.04,                    !- Loads Convergence Tolerance Value",
		"    0.4,                     !- Temperature Convergence Tolerance Value {deltaC}",
		"    FullExterior,            !- Solar Distribution",
		"    25,                      !- Maximum Number of Warmup Days",
		"    6;                       !- Minimum Number of Warmup Days",

		"  Timestep,6;",

		"  Site:Location,",
		"    CHICAGO_IL_USA TMY2-94846,  !- Name",
		"    41.78,                   !- Latitude {deg}",
		"    -87.75,                  !- Longitude {deg}",
		"    -6.00,                   !- Time Zone {hr}",
		"    190.00;                  !- Elevation {m}",

		"  SizingPeriod:DesignDay,",
		"    CHICAGO_IL_USA Annual Heating 99% Design Conditions DB,  !- Name",
		"    1,                       !- Month",
		"    21,                      !- Day of Month",
		"    WinterDesignDay,         !- Day Type",
		"    -17.3,                   !- Maximum Dry-Bulb Temperature {C}",
		"    0.0,                     !- Daily Dry-Bulb Temperature Range {deltaC}",
		"    ,                        !- Dry-Bulb Temperature Range Modifier Type",
		"    ,                        !- Dry-Bulb Temperature Range Modifier Day Schedule Name",
		"    Wetbulb,                 !- Humidity Condition Type",
		"    -17.3,                   !- Wetbulb or DewPoint at Maximum Dry-Bulb {C}",
		"    ,                        !- Humidity Condition Day Schedule Name",
		"    ,                        !- Humidity Ratio at Maximum Dry-Bulb {kgWater/kgDryAir}",
		"    ,                        !- Enthalpy at Maximum Dry-Bulb {J/kg}",
		"    ,                        !- Daily Wet-Bulb Temperature Range {deltaC}",
		"    99063.,                  !- Barometric Pressure {Pa}",
		"    4.9,                     !- Wind Speed {m/s}",
		"    270,                     !- Wind Direction {deg}",
		"    No,                      !- Rain Indicator",
		"    No,                      !- Snow Indicator",
		"    No,                      !- Daylight Saving Time Indicator",
		"    ASHRAEClearSky,          !- Solar Model Indicator",
		"    ,                        !- Beam Solar Day Schedule Name",
		"    ,                        !- Diffuse Solar Day Schedule Name",
		"    ,                        !- ASHRAE Clear Sky Optical Depth for Beam Irradiance (taub) {dimensionless}",
		"    ,                        !- ASHRAE Clear Sky Optical Depth for Diffuse Irradiance (taud) {dimensionless}",
		"    1;                       !- Sky Clearness",

		"  SizingPeriod:DesignDay,",
		"    CHICAGO_IL_USA Annual Cooling 1% Design Conditions DB/MCWB,  !- Name",
		"    7,                       !- Month",
		"    21,                      !- Day of Month",
		"    SummerDesignDay,         !- Day Type",
		"    31.5,                    !- Maximum Dry-Bulb Temperature {C}",
		"    10.7,                    !- Daily Dry-Bulb Temperature Range {deltaC}",
		"    ,                        !- Dry-Bulb Temperature Range Modifier Type",
		"    ,                        !- Dry-Bulb Temperature Range Modifier Day Schedule Name",
		"    Wetbulb,                 !- Humidity Condition Type",
		"    23.0,                    !- Wetbulb or DewPoint at Maximum Dry-Bulb {C}",
		"    ,                        !- Humidity Condition Day Schedule Name",
		"    ,                        !- Humidity Ratio at Maximum Dry-Bulb {kgWater/kgDryAir}",
		"    ,                        !- Enthalpy at Maximum Dry-Bulb {J/kg}",
		"    ,                        !- Daily Wet-Bulb Temperature Range {deltaC}",
		"    99063.,                  !- Barometric Pressure {Pa}",
		"    5.3,                     !- Wind Speed {m/s}",
		"    230,                     !- Wind Direction {deg}",
		"    No,                      !- Rain Indicator",
		"    No,                      !- Snow Indicator",
		"    No,                      !- Daylight Saving Time Indicator",
		"    ASHRAEClearSky,          !- Solar Model Indicator",
		"    ,                        !- Beam Solar Day Schedule Name",
		"    ,                        !- Diffuse Solar Day Schedule Name",
		"    ,                        !- ASHRAE Clear Sky Optical Depth for Beam Irradiance (taub) {dimensionless}",
		"    ,                        !- ASHRAE Clear Sky Optical Depth for Diffuse Irradiance (taud) {dimensionless}",
		"    1.0;                     !- Sky Clearness",

		"  RunPeriod,",
		"    ,                        !- Name",
		"    7,                       !- Begin Month",
		"    5,                       !- Begin Day of Month",
		"    7,                       !- End Month",
		"    12,                      !- End Day of Month",
		"    UseWeatherFile,          !- Day of Week for Start Day",
		"    Yes,                     !- Use Weather File Holidays and Special Days",
		"    Yes,                     !- Use Weather File Daylight Saving Period",
		"    No,                      !- Apply Weekend Holiday Rule",
		"    Yes,                     !- Use Weather File Rain Indicators",
		"    Yes;                     !- Use Weather File Snow Indicators",


		"  Site:GroundTemperature:BuildingSurface,20.03,20.03,20.13,20.30,20.43,20.52,20.62,20.77,20.78,20.55,20.44,20.20;",


		"  ScheduleTypeLimits,",
		"    Any Number;              !- Name",

		"  ScheduleTypeLimits,",
		"    Fraction,                !- Name",
		"    0.0,                     !- Lower Limit Value",
		"    1.0,                     !- Upper Limit Value",
		"    CONTINUOUS;              !- Numeric Type",

		"  ScheduleTypeLimits,",
		"    Temperature,             !- Name",
		"    -60,                     !- Lower Limit Value",
		"    200,                     !- Upper Limit Value",
		"    CONTINUOUS,              !- Numeric Type",
		"    Temperature;             !- Unit Type",

		"  ScheduleTypeLimits,",
		"    Control Type,            !- Name",
		"    0,                       !- Lower Limit Value",
		"    4,                       !- Upper Limit Value",
		"    DISCRETE;                !- Numeric Type",

		"  ScheduleTypeLimits,",
		"    On/Off,                  !- Name",
		"    0,                       !- Lower Limit Value",
		"    1,                       !- Upper Limit Value",
		"    DISCRETE;                !- Numeric Type",

		"  Schedule:Compact,",
		"    INFIL-SCH,               !- Name",
		"    Fraction,                !- Schedule Type Limits Name",
		"    Through: 12/31,          !- Field 1",
		"    For: AllDays,            !- Field 2",
		"    Until: 24:00,1.0;        !- Field 3",

		"  Schedule:Compact,",
		"    ON,                      !- Name",
		"    Fraction,                !- Schedule Type Limits Name",
		"    Through: 12/31,          !- Field 1",
		"    For: AllDays,            !- Field 2",
		"    Until: 24:00,1.0;        !- Field 3",

		"  Schedule:Compact,",
		"    CW Loop Temp Schedule,   !- Name",
		"    Temperature,             !- Schedule Type Limits Name",
		"    Through: 12/31,          !- Field 1",
		"    For: AllDays,            !- Field 2",
		"    Until: 24:00,6.67;       !- Field 3",

		"  Schedule:Compact,",
		"    HW Loop Temp Schedule,   !- Name",
		"    Temperature,             !- Schedule Type Limits Name",
		"    Through: 12/31,          !- Field 1",
		"    For: AllDays,            !- Field 2",
		"    Until: 24:00,60.0;       !- Field 3",

		"  Schedule:Compact,",
		"    VentSlabMaxOA,           !- Name",
		"    Any Number,              !- Schedule Type Limits Name",
		"    Through: 12/31,          !- Field 1",
		"    For: AllDays,            !- Field 2",
		"    Until: 24:00,1.0;        !- Field 3",

		"  Schedule:Compact,",
		"    VentSlabHotHighAir,      !- Name",
		"    Temperature,             !- Schedule Type Limits Name",
		"    Through: 12/31,          !- Field 1",
		"    For: AllDays,            !- Field 2",
		"    Until: 24:00,30;         !- Field 3",

		"  Schedule:Compact,",
		"    VentSlabHotLowAir,       !- Name",
		"    Temperature,             !- Schedule Type Limits Name",
		"    Through: 12/31,          !- Field 1",
		"    For: AllDays,            !- Field 2",
		"    Until: 24:00,25;         !- Field 3",

		"  Schedule:Compact,",
		"    VentSlabHotLowControl,   !- Name",
		"    Temperature,             !- Schedule Type Limits Name",
		"    Through: 12/31,          !- Field 1",
		"    For: AllDays,            !- Field 2",
		"    Until: 24:00,10;         !- Field 3",

		"  Schedule:Compact,",
		"    VentSlabHotHighControl,  !- Name",
		"    Temperature,             !- Schedule Type Limits Name",
		"    Through: 12/31,          !- Field 1",
		"    For: AllDays,            !- Field 2",
		"    Until: 24:00,20;         !- Field 3",

		"  Schedule:Compact,",
		"    VentSlabCoolHighControl, !- Name",
		"    Temperature,             !- Schedule Type Limits Name",
		"    Through: 12/31,          !- Field 1",
		"    For: AllDays,            !- Field 2",
		"    Until: 24:00,30;         !- Field 3",

		"  Schedule:Compact,",
		"    VentSlabCoolHighAir,     !- Name",
		"    Temperature,             !- Schedule Type Limits Name",
		"    Through: 12/31,          !- Field 1",
		"    For: AllDays,            !- Field 2",
		"    Until: 24:00,23;         !- Field 3",

		"  Schedule:Compact,",
		"    VentSlabCoolLowAir,      !- Name",
		"    Temperature,             !- Schedule Type Limits Name",
		"    Through: 12/31,          !- Field 1",
		"    For: AllDays,            !- Field 2",
		"    Until: 24:00,17;         !- Field 3",

		"  Schedule:Compact,",
		"    FanAndCoilAvailSched,    !- Name",
		"    Fraction,                !- Schedule Type Limits Name",
		"    Through: 12/31,          !- Field 1",
		"    For: AllDays,            !- Field 2",
		"    Until: 24:00,1.0;        !- Field 3",

		"  Schedule:Compact,",
		"    VentSlabCoolLowControl,  !- Name",
		"    Temperature,             !- Schedule Type Limits Name",
		"    Through: 12/31,          !- Field 1",
		"    For: AllDays,            !- Field 2",
		"    Until: 24:00,26;         !- Field 3",

		"  Schedule:Compact,",
		"    VentSlabCoolLowControl2, !- Name",
		"    Temperature,             !- Schedule Type Limits Name",
		"    Through: 12/31,          !- Field 1",
		"    For: AllDays,            !- Field 2",
		"    Until: 24:00,20;         !- Field 3",

		"  Material,",
		"    WD10,                    !- Name",
		"    MediumSmooth,            !- Roughness",
		"    0.667,                   !- Thickness {m}",
		"    0.115,                   !- Conductivity {W/m-K}",
		"    513,                     !- Density {kg/m3}",
		"    1381,                    !- Specific Heat {J/kg-K}",
		"    0.9,                     !- Thermal Absorptance",
		"    0.78,                    !- Solar Absorptance",
		"    0.78;                    !- Visible Absorptance",

		"  Material,",
		"    RG01,                    !- Name",
		"    Rough,                   !- Roughness",
		"    1.2700000E-02,           !- Thickness {m}",
		"    1.442000,                !- Conductivity {W/m-K}",
		"    881.0000,                !- Density {kg/m3}",
		"    1674.000,                !- Specific Heat {J/kg-K}",
		"    0.9000000,               !- Thermal Absorptance",
		"    0.6500000,               !- Solar Absorptance",
		"    0.6500000;               !- Visible Absorptance",

		"  Material,",
		"    BR01,                    !- Name",
		"    VeryRough,               !- Roughness",
		"    0.009,                   !- Thickness {m}",
		"    0.1620000,               !- Conductivity {W/m-K}",
		"    1121.000,                !- Density {kg/m3}",
		"    1464.000,                !- Specific Heat {J/kg-K}",
		"    0.9000000,               !- Thermal Absorptance",
		"    0.7000000,               !- Solar Absorptance",
		"    0.7000000;               !- Visible Absorptance",

		"  Material,",
		"    IN46,                    !- Name",
		"    VeryRough,               !- Roughness",
		"    0.09,                    !- Thickness {m}",
		"    2.3000000E-02,           !- Conductivity {W/m-K}",
		"    24.00000,                !- Density {kg/m3}",
		"    1590.000,                !- Specific Heat {J/kg-K}",
		"    0.9000000,               !- Thermal Absorptance",
		"    0.5000000,               !- Solar Absorptance",
		"    0.5000000;               !- Visible Absorptance",

		"  Material,",
		"    WD01,                    !- Name",
		"    MediumSmooth,            !- Roughness",
		"    1.9099999E-02,           !- Thickness {m}",
		"    0.1150000,               !- Conductivity {W/m-K}",
		"    513.0000,                !- Density {kg/m3}",
		"    1381.000,                !- Specific Heat {J/kg-K}",
		"    0.9000000,               !- Thermal Absorptance",
		"    0.7800000,               !- Solar Absorptance",
		"    0.7800000;               !- Visible Absorptance",

		"  Material,",
		"    PW03,                    !- Name",
		"    MediumSmooth,            !- Roughness",
		"    1.2700000E-02,           !- Thickness {m}",
		"    0.1150000,               !- Conductivity {W/m-K}",
		"    545.0000,                !- Density {kg/m3}",
		"    1213.000,                !- Specific Heat {J/kg-K}",
		"    0.9000000,               !- Thermal Absorptance",
		"    0.7800000,               !- Solar Absorptance",
		"    0.7800000;               !- Visible Absorptance",

		"  Material,",
		"    IN02,                    !- Name",
		"    Rough,                   !- Roughness",
		"    9.0099998E-02,           !- Thickness {m}",
		"    4.3000001E-02,           !- Conductivity {W/m-K}",
		"    10.00000,                !- Density {kg/m3}",
		"    837.0000,                !- Specific Heat {J/kg-K}",
		"    0.9000000,               !- Thermal Absorptance",
		"    0.7500000,               !- Solar Absorptance",
		"    0.7500000;               !- Visible Absorptance",

		"  Material,",
		"    GP01,                    !- Name",
		"    MediumSmooth,            !- Roughness",
		"    1.2700000E-02,           !- Thickness {m}",
		"    0.1600000,               !- Conductivity {W/m-K}",
		"    801.0000,                !- Density {kg/m3}",
		"    837.0000,                !- Specific Heat {J/kg-K}",
		"    0.9000000,               !- Thermal Absorptance",
		"    0.7500000,               !- Solar Absorptance",
		"    0.7500000;               !- Visible Absorptance",

		"  Material,",
		"    GP02,                    !- Name",
		"    MediumSmooth,            !- Roughness",
		"    1.5900001E-02,           !- Thickness {m}",
		"    0.1600000,               !- Conductivity {W/m-K}",
		"    801.0000,                !- Density {kg/m3}",
		"    837.0000,                !- Specific Heat {J/kg-K}",
		"    0.9000000,               !- Thermal Absorptance",
		"    0.7500000,               !- Solar Absorptance",
		"    0.7500000;               !- Visible Absorptance",

		"  Material,",
		"    CC03,                    !- Name",
		"    MediumRough,             !- Roughness",
		"    0.1016000,               !- Thickness {m}",
		"    1.310000,                !- Conductivity {W/m-K}",
		"    2243.000,                !- Density {kg/m3}",
		"    837.0000,                !- Specific Heat {J/kg-K}",
		"    0.9000000,               !- Thermal Absorptance",
		"    0.6500000,               !- Solar Absorptance",
		"    0.6500000;               !- Visible Absorptance",

		"  Material,",
		"    COnc,                    !- Name",
		"    VeryRough,               !- Roughness",
		"    0.025,                   !- Thickness {m}",
		"    0.72,                    !- Conductivity {W/m-K}",
		"    1860,                    !- Density {kg/m3}",
		"    780,                     !- Specific Heat {J/kg-K}",
		"    0.9,                     !- Thermal Absorptance",
		"    0.7,                     !- Solar Absorptance",
		"    0.7;                     !- Visible Absorptance",

		"  Material,",
		"    FINISH FLOORING - TILE 1 / 16 IN,  !- Name",
		"    Smooth,                  !- Roughness",
		"    1.6000000E-03,           !- Thickness {m}",
		"    0.1700000,               !- Conductivity {W/m-K}",
		"    1922.210,                !- Density {kg/m3}",
		"    1250.000,                !- Specific Heat {J/kg-K}",
		"    0.9000000,               !- Thermal Absorptance",
		"    0.5000000,               !- Solar Absorptance",
		"    0.5000000;               !- Visible Absorptance",

		"  Material,",
		"    GYP1,                    !- Name",
		"    MediumRough,             !- Roughness",
		"    1.2700000E-02,           !- Thickness {m}",
		"    7.8450000E-01,           !- Conductivity {W/m-K}",
		"    1842.1221,               !- Density {kg/m3}",
		"    988.000,                 !- Specific Heat {J/kg-K}",
		"    0.9000000,               !- Thermal Absorptance",
		"    0.5000000,               !- Solar Absorptance",
		"    0.5000000;               !- Visible Absorptance",

		"  Material,",
		"    GYP2,                    !- Name",
		"    MediumRough,             !- Roughness",
		"    1.9050000E-02,           !- Thickness {m}",
		"    7.8450000E-01,           !- Conductivity {W/m-K}",
		"    1842.1221,               !- Density {kg/m3}",
		"    988.000,                 !- Specific Heat {J/kg-K}",
		"    0.9000000,               !- Thermal Absorptance",
		"    0.5000000,               !- Solar Absorptance",
		"    0.5000000;               !- Visible Absorptance",

		"  Material,",
		"    INS - EXPANDED EXT POLYSTYRENE R12,  !- Name",
		"    Rough,                   !- Roughness",
		"    0.07,                    !- Thickness {m}",
		"    2.0000000E-02,           !- Conductivity {W/m-K}",
		"    56.06000,                !- Density {kg/m3}",
		"    1210.000,                !- Specific Heat {J/kg-K}",
		"    0.9000000,               !- Thermal Absorptance",
		"    0.5000000,               !- Solar Absorptance",
		"    0.5000000;               !- Visible Absorptance",

		"  Material,",
		"    CONCRETE,                !- Name",
		"    MediumRough,             !- Roughness",
		"    0.5000000,               !- Thickness {m}",
		"    1.290000,                !- Conductivity {W/m-K}",
		"    2242.580,                !- Density {kg/m3}",
		"    830.00000,               !- Specific Heat {J/kg-K}",
		"    0.9000000,               !- Thermal Absorptance",
		"    0.6000000,               !- Solar Absorptance",
		"    0.6000000;               !- Visible Absorptance",

		"  Material,",
		"    CLN-INS,                 !- Name",
		"    Rough,                   !- Roughness",
		"    0.005,                   !- Thickness {m}",
		"    0.5,                     !- Conductivity {W/m-K}",
		"    56.06000,                !- Density {kg/m3}",
		"    1210.000,                !- Specific Heat {J/kg-K}",
		"    0.9000000,               !- Thermal Absorptance",
		"    0.5000000,               !- Solar Absorptance",
		"    0.5000000;               !- Visible Absorptance",

		"  Material:NoMass,",
		"    CP01,                    !- Name",
		"    Rough,                   !- Roughness",
		"    0.3670000,               !- Thermal Resistance {m2-K/W}",
		"    0.9000000,               !- Thermal Absorptance",
		"    0.7500000,               !- Solar Absorptance",
		"    0.7500000;               !- Visible Absorptance",

		"  Material:NoMass,",
		"    MAT-SB-U,                !- Name",
		"    Rough,                   !- Roughness",
		"    0.117406666,             !- Thermal Resistance {m2-K/W}",
		"    0.65,                    !- Thermal Absorptance",
		"    0.65,                    !- Solar Absorptance",
		"    0.65;                    !- Visible Absorptance",

		"  Material:NoMass,",
		"    MAT-CLNG-1,              !- Name",
		"    Rough,                   !- Roughness",
		"    0.652259290,             !- Thermal Resistance {m2-K/W}",
		"    0.65,                    !- Thermal Absorptance",
		"    0.65,                    !- Solar Absorptance",
		"    0.65;                    !- Visible Absorptance",

		"  Material:NoMass,",
		"    MAT-FLOOR-1,             !- Name",
		"    Rough,                   !- Roughness",
		"    3.522199631,             !- Thermal Resistance {m2-K/W}",
		"    0.65,                    !- Thermal Absorptance",
		"    0.65,                    !- Solar Absorptance",
		"    0.65;                    !- Visible Absorptance",


		"  Material:AirGap,",
		"    AL21,                    !- Name",
		"    0.1570000;               !- Thermal Resistance {m2-K/W}",

		"  Material:AirGap,",
		"    AL23,                    !- Name",
		"    0.1530000;               !- Thermal Resistance {m2-K/W}",


		"  WindowMaterial:Glazing,",
		"    CLEAR 3MM,               !- Name",
		"    SpectralAverage,         !- Optical Data Type",
		"    ,                        !- Window Glass Spectral Data Set Name",
		"    0.003,                   !- Thickness {m}",
		"    0.837,                   !- Solar Transmittance at Normal Incidence",
		"    0.075,                   !- Front Side Solar Reflectance at Normal Incidence",
		"    0.075,                   !- Back Side Solar Reflectance at Normal Incidence",
		"    0.898,                   !- Visible Transmittance at Normal Incidence",
		"    0.081,                   !- Front Side Visible Reflectance at Normal Incidence",
		"    0.081,                   !- Back Side Visible Reflectance at Normal Incidence",
		"    0.0,                     !- Infrared Transmittance at Normal Incidence",
		"    0.84,                    !- Front Side Infrared Hemispherical Emissivity",
		"    0.84,                    !- Back Side Infrared Hemispherical Emissivity",
		"    0.9;                     !- Conductivity {W/m-K}",

		"  WindowMaterial:Glazing,",
		"    GREY 3MM,                !- Name",
		"    SpectralAverage,         !- Optical Data Type",
		"    ,                        !- Window Glass Spectral Data Set Name",
		"    0.003,                   !- Thickness {m}",
		"    0.626,                   !- Solar Transmittance at Normal Incidence",
		"    0.061,                   !- Front Side Solar Reflectance at Normal Incidence",
		"    0.061,                   !- Back Side Solar Reflectance at Normal Incidence",
		"    0.611,                   !- Visible Transmittance at Normal Incidence",
		"    0.061,                   !- Front Side Visible Reflectance at Normal Incidence",
		"    0.061,                   !- Back Side Visible Reflectance at Normal Incidence",
		"    0.0,                     !- Infrared Transmittance at Normal Incidence",
		"    0.84,                    !- Front Side Infrared Hemispherical Emissivity",
		"    0.84,                    !- Back Side Infrared Hemispherical Emissivity",
		"    0.9;                     !- Conductivity {W/m-K}",

		"  WindowMaterial:Glazing,",
		"    CLEAR 6MM,               !- Name",
		"    SpectralAverage,         !- Optical Data Type",
		"    ,                        !- Window Glass Spectral Data Set Name",
		"    0.006,                   !- Thickness {m}",
		"    0.775,                   !- Solar Transmittance at Normal Incidence",
		"    0.071,                   !- Front Side Solar Reflectance at Normal Incidence",
		"    0.071,                   !- Back Side Solar Reflectance at Normal Incidence",
		"    0.881,                   !- Visible Transmittance at Normal Incidence",
		"    0.080,                   !- Front Side Visible Reflectance at Normal Incidence",
		"    0.080,                   !- Back Side Visible Reflectance at Normal Incidence",
		"    0.0,                     !- Infrared Transmittance at Normal Incidence",
		"    0.84,                    !- Front Side Infrared Hemispherical Emissivity",
		"    0.84,                    !- Back Side Infrared Hemispherical Emissivity",
		"    0.9;                     !- Conductivity {W/m-K}",

		"  WindowMaterial:Glazing,",
		"    LoE CLEAR 6MM,           !- Name",
		"    SpectralAverage,         !- Optical Data Type",
		"    ,                        !- Window Glass Spectral Data Set Name",
		"    0.006,                   !- Thickness {m}",
		"    0.600,                   !- Solar Transmittance at Normal Incidence",
		"    0.170,                   !- Front Side Solar Reflectance at Normal Incidence",
		"    0.220,                   !- Back Side Solar Reflectance at Normal Incidence",
		"    0.840,                   !- Visible Transmittance at Normal Incidence",
		"    0.055,                   !- Front Side Visible Reflectance at Normal Incidence",
		"    0.078,                   !- Back Side Visible Reflectance at Normal Incidence",
		"    0.0,                     !- Infrared Transmittance at Normal Incidence",
		"    0.84,                    !- Front Side Infrared Hemispherical Emissivity",
		"    0.10,                    !- Back Side Infrared Hemispherical Emissivity",
		"    0.9;                     !- Conductivity {W/m-K}",


		"  WindowMaterial:Gas,",
		"    AIR 6MM,                 !- Name",
		"    Air,                     !- Gas Type",
		"    0.0063;                  !- Thickness {m}",

		"  WindowMaterial:Gas,",
		"    AIR 13MM,                !- Name",
		"    Air,                     !- Gas Type",
		"    0.0127;                  !- Thickness {m}",

		"  WindowMaterial:Gas,",
		"    ARGON 13MM,              !- Name",
		"    Argon,                   !- Gas Type",
		"    0.0127;                  !- Thickness {m}",


		"  Construction,",
		"    ROOF-1,                  !- Name",
		"    RG01,                    !- Outside Layer",
		"    BR01,                    !- Layer 2",
		"    IN46,                    !- Layer 3",
		"    WD01;                    !- Layer 4",

		"  Construction,",
		"    WALL-1,                  !- Name",
		"    WD01,                    !- Outside Layer",
		"    PW03,                    !- Layer 2",
		"    IN02,                    !- Layer 3",
		"    GP01;                    !- Layer 4",

		"  Construction,",
		"    CLNG-1,                  !- Name",
		"    MAT-CLNG-1;              !- Outside Layer",

		"  Construction,",
		"    SB-U,                    !- Name",
		"    MAT-SB-U;                !- Outside Layer",

		"  Construction,",
		"    FLOOR-1,                 !- Name",
		"    MAT-FLOOR-1;             !- Outside Layer",

		"  Construction,",
		"    FLOOR-SLAB-1,            !- Name",
		"    CONCRETE,                !- Outside Layer",
		"    INS - EXPANDED EXT POLYSTYRENE R12,  !- Layer 2",
		"    CONC,                    !- Layer 3",
		"    FINISH FLOORING - TILE 1 / 16 IN;  !- Layer 4",

		"  Construction,",
		"    INT-WALL-1,              !- Name",
		"    GP02,                    !- Outside Layer",
		"    AL21,                    !- Layer 2",
		"    GP02;                    !- Layer 3",

		"  Construction,",
		"    Dbl Clr 3mm/13mm Air,    !- Name",
		"    CLEAR 3MM,               !- Outside Layer",
		"    AIR 13MM,                !- Layer 2",
		"    CLEAR 3MM;               !- Layer 3",

		"  Construction,",
		"    Dbl Clr 3mm/13mm Arg,    !- Name",
		"    CLEAR 3MM,               !- Outside Layer",
		"    ARGON 13MM,              !- Layer 2",
		"    CLEAR 3MM;               !- Layer 3",

		"  Construction,",
		"    Sgl Grey 3mm,            !- Name",
		"    GREY 3MM;                !- Outside Layer",

		"  Construction,",
		"    Dbl Clr 6mm/6mm Air,     !- Name",
		"    CLEAR 6MM,               !- Outside Layer",
		"    AIR 6MM,                 !- Layer 2",
		"    CLEAR 6MM;               !- Layer 3",

		"  Construction,",
		"    Dbl LoE (e2=.1) Clr 6mm/6mm Air,  !- Name",
		"    LoE CLEAR 6MM,           !- Outside Layer",
		"    AIR 6MM,                 !- Layer 2",
		"    CLEAR 6MM;               !- Layer 3",


		"  Construction:InternalSource,",
		"    Ceiling with Radiant,    !- Name",
		"    2,                       !- Source Present After Layer Number",
		"    2,                       !- Temperature Calculation Requested After Layer Number",
		"    1,                       !- Dimensions for the CTF Calculation",
		"    0.1524,                  !- Tube Spacing {m}",
		"    CLN-INS,                 !- Outside Layer",
		"    GYP1,                    !- Layer 2",
		"    GYP2,                    !- Layer 3",
		"    MAT-CLNG-1;              !- Layer 4",

		"  Construction:InternalSource,",
		"    reverseCeiling with Radiant,  !- Name",
		"    2,                       !- Source Present After Layer Number",
		"    2,                       !- Temperature Calculation Requested After Layer Number",
		"    1,                       !- Dimensions for the CTF Calculation",
		"    0.1524,                  !- Tube Spacing {m}",
		"    MAT-CLNG-1,              !- Outside Layer",
		"    GYP2,                    !- Layer 2",
		"    GYP1,                    !- Layer 3",
		"    CLN-INS;                 !- Layer 4",

		"  Construction:InternalSource,",
		"    Floor with Radiant,      !- Name",
		"    2,                       !- Source Present After Layer Number",
		"    2,                       !- Temperature Calculation Requested After Layer Number",
		"    1,                       !- Dimensions for the CTF Calculation",
		"    0.1524,                  !- Tube Spacing {m}",
		"    INS - EXPANDED EXT POLYSTYRENE R12,  !- Outside Layer",
		"    CONC,                    !- Layer 2",
		"    CONC,                    !- Layer 3",
		"    FINISH FLOORING - TILE 1 / 16 IN;  !- Layer 4",


		"  GlobalGeometryRules,",
		"    UpperLeftCorner,         !- Starting Vertex Position",
		"    CounterClockWise,        !- Vertex Entry Direction",
		"    relative;                !- Coordinate System",


		"  Zone,",
		"    PLENUM-1,                !- Name",
		"    0,                       !- Direction of Relative North {deg}",
		"    0,                       !- X Origin {m}",
		"    0,                       !- Y Origin {m}",
		"    0,                       !- Z Origin {m}",
		"    1,                       !- Type",
		"    1,                       !- Multiplier",
		"    0.609600067,             !- Ceiling Height {m}",
		"    283.2;                   !- Volume {m3}",

		"  Zone,",
		"    SPACE1-1,                !- Name",
		"    0,                       !- Direction of Relative North {deg}",
		"    0,                       !- X Origin {m}",
		"    0,                       !- Y Origin {m}",
		"    0,                       !- Z Origin {m}",
		"    1,                       !- Type",
		"    1,                       !- Multiplier",
		"    2.438400269,             !- Ceiling Height {m}",
		"    239.247360229;           !- Volume {m3}",

		"  Zone,",
		"    SPACE2-1,                !- Name",
		"    0,                       !- Direction of Relative North {deg}",
		"    0,                       !- X Origin {m}",
		"    0,                       !- Y Origin {m}",
		"    0,                       !- Z Origin {m}",
		"    1,                       !- Type",
		"    1,                       !- Multiplier",
		"    2.438400269,             !- Ceiling Height {m}",
		"    103.311355591;           !- Volume {m3}",

		"  Zone,",
		"    SPACE3-1,                !- Name",
		"    0,                       !- Direction of Relative North {deg}",
		"    0,                       !- X Origin {m}",
		"    0,                       !- Y Origin {m}",
		"    0,                       !- Z Origin {m}",
		"    1,                       !- Type",
		"    1,                       !- Multiplier",
		"    2.438400269,             !- Ceiling Height {m}",
		"    239.247360229;           !- Volume {m3}",

		"  Zone,",
		"    SPACE4-1,                !- Name",
		"    0,                       !- Direction of Relative North {deg}",
		"    0,                       !- X Origin {m}",
		"    0,                       !- Y Origin {m}",
		"    0,                       !- Z Origin {m}",
		"    1,                       !- Type",
		"    1,                       !- Multiplier",
		"    2.438400269,             !- Ceiling Height {m}",
		"    103.311355591;           !- Volume {m3}",

		"  Zone,",
		"    SPACE5-1,                !- Name",
		"    0,                       !- Direction of Relative North {deg}",
		"    0,                       !- X Origin {m}",
		"    0,                       !- Y Origin {m}",
		"    0,                       !- Z Origin {m}",
		"    1,                       !- Type",
		"    1,                       !- Multiplier",
		"    2.438400269,             !- Ceiling Height {m}",
		"    447.682556152;           !- Volume {m3}",


		"  BuildingSurface:Detailed,",
		"    WALL-1PF,                !- Name",
		"    WALL,                    !- Surface Type",
		"    WALL-1,                  !- Construction Name",
		"    PLENUM-1,                !- Zone Name",
		"    Outdoors,                !- Outside Boundary Condition",
		"    ,                        !- Outside Boundary Condition Object",
		"    SunExposed,              !- Sun Exposure",
		"    WindExposed,             !- Wind Exposure",
		"    0.50000,                 !- View Factor to Ground",
		"    4,                       !- Number of Vertices",
		"    0.0,0.0,3.0,  !- X,Y,Z ==> Vertex 1 {m}",
		"    0.0,0.0,2.4,  !- X,Y,Z ==> Vertex 2 {m}",
		"    30.5,0.0,2.4,  !- X,Y,Z ==> Vertex 3 {m}",
		"    30.5,0.0,3.0;  !- X,Y,Z ==> Vertex 4 {m}",

		"  BuildingSurface:Detailed,",
		"    WALL-1PR,                !- Name",
		"    WALL,                    !- Surface Type",
		"    WALL-1,                  !- Construction Name",
		"    PLENUM-1,                !- Zone Name",
		"    Outdoors,                !- Outside Boundary Condition",
		"    ,                        !- Outside Boundary Condition Object",
		"    SunExposed,              !- Sun Exposure",
		"    WindExposed,             !- Wind Exposure",
		"    0.50000,                 !- View Factor to Ground",
		"    4,                       !- Number of Vertices",
		"    30.5,0.0,3.0,  !- X,Y,Z ==> Vertex 1 {m}",
		"    30.5,0.0,2.4,  !- X,Y,Z ==> Vertex 2 {m}",
		"    30.5,15.2,2.4,  !- X,Y,Z ==> Vertex 3 {m}",
		"    30.5,15.2,3.0;  !- X,Y,Z ==> Vertex 4 {m}",

		"  BuildingSurface:Detailed,",
		"    WALL-1PB,                !- Name",
		"    WALL,                    !- Surface Type",
		"    WALL-1,                  !- Construction Name",
		"    PLENUM-1,                !- Zone Name",
		"    Outdoors,                !- Outside Boundary Condition",
		"    ,                        !- Outside Boundary Condition Object",
		"    SunExposed,              !- Sun Exposure",
		"    WindExposed,             !- Wind Exposure",
		"    0.50000,                 !- View Factor to Ground",
		"    4,                       !- Number of Vertices",
		"    30.5,15.2,3.0,  !- X,Y,Z ==> Vertex 1 {m}",
		"    30.5,15.2,2.4,  !- X,Y,Z ==> Vertex 2 {m}",
		"    0.0,15.2,2.4,  !- X,Y,Z ==> Vertex 3 {m}",
		"    0.0,15.2,3.0;  !- X,Y,Z ==> Vertex 4 {m}",

		"  BuildingSurface:Detailed,",
		"    WALL-1PL,                !- Name",
		"    WALL,                    !- Surface Type",
		"    WALL-1,                  !- Construction Name",
		"    PLENUM-1,                !- Zone Name",
		"    Outdoors,                !- Outside Boundary Condition",
		"    ,                        !- Outside Boundary Condition Object",
		"    SunExposed,              !- Sun Exposure",
		"    WindExposed,             !- Wind Exposure",
		"    0.50000,                 !- View Factor to Ground",
		"    4,                       !- Number of Vertices",
		"    0.0,15.2,3.0,  !- X,Y,Z ==> Vertex 1 {m}",
		"    0.0,15.2,2.4,  !- X,Y,Z ==> Vertex 2 {m}",
		"    0.0,0.0,2.4,  !- X,Y,Z ==> Vertex 3 {m}",
		"    0.0,0.0,3.0;  !- X,Y,Z ==> Vertex 4 {m}",

		"  BuildingSurface:Detailed,",
		"    TOP-1,                   !- Name",
		"    ROOF,                    !- Surface Type",
		"    ROOF-1,                  !- Construction Name",
		"    PLENUM-1,                !- Zone Name",
		"    Outdoors,                !- Outside Boundary Condition",
		"    ,                        !- Outside Boundary Condition Object",
		"    SunExposed,              !- Sun Exposure",
		"    WindExposed,             !- Wind Exposure",
		"    0.00000,                 !- View Factor to Ground",
		"    4,                       !- Number of Vertices",
		"    0.0,15.2,3.0,  !- X,Y,Z ==> Vertex 1 {m}",
		"    0.0,0.0,3.0,  !- X,Y,Z ==> Vertex 2 {m}",
		"    30.5,0.0,3.0,  !- X,Y,Z ==> Vertex 3 {m}",
		"    30.5,15.2,3.0;  !- X,Y,Z ==> Vertex 4 {m}",

		"  BuildingSurface:Detailed,",
		"    C1-1P,                   !- Name",
		"    FLOOR,                   !- Surface Type",
		"    reverseceiling with Radiant,  !- Construction Name",
		"    PLENUM-1,                !- Zone Name",
		"    Surface,                 !- Outside Boundary Condition",
		"    C1-1,                    !- Outside Boundary Condition Object",
		"    NoSun,                   !- Sun Exposure",
		"    NoWind,                  !- Wind Exposure",
		"    0.0,                     !- View Factor to Ground",
		"    4,                       !- Number of Vertices",
		"    26.8,3.7,2.4,  !- X,Y,Z ==> Vertex 1 {m}",
		"    30.5,0.0,2.4,  !- X,Y,Z ==> Vertex 2 {m}",
		"    0.0,0.0,2.4,  !- X,Y,Z ==> Vertex 3 {m}",
		"    3.7,3.7,2.4;  !- X,Y,Z ==> Vertex 4 {m}",

		"  BuildingSurface:Detailed,",
		"    C2-1P,                   !- Name",
		"    FLOOR,                   !- Surface Type",
		"    reverseCeiling with Radiant,  !- Construction Name",
		"    PLENUM-1,                !- Zone Name",
		"    Surface,                 !- Outside Boundary Condition",
		"    C2-1,                    !- Outside Boundary Condition Object",
		"    NoSun,                   !- Sun Exposure",
		"    NoWind,                  !- Wind Exposure",
		"    0.0,                     !- View Factor to Ground",
		"    4,                       !- Number of Vertices",
		"    26.8,11.6,2.4,  !- X,Y,Z ==> Vertex 1 {m}",
		"    30.5,15.2,2.4,  !- X,Y,Z ==> Vertex 2 {m}",
		"    30.5,0.0,2.4,  !- X,Y,Z ==> Vertex 3 {m}",
		"    26.8,3.7,2.4;  !- X,Y,Z ==> Vertex 4 {m}",

		"  BuildingSurface:Detailed,",
		"    C3-1P,                   !- Name",
		"    FLOOR,                   !- Surface Type",
		"    reverseCeiling with Radiant,  !- Construction Name",
		"    PLENUM-1,                !- Zone Name",
		"    Surface,                 !- Outside Boundary Condition",
		"    C3-1,                    !- Outside Boundary Condition Object",
		"    NoSun,                   !- Sun Exposure",
		"    NoWind,                  !- Wind Exposure",
		"    0.0,                     !- View Factor to Ground",
		"    4,                       !- Number of Vertices",
		"    26.8,11.6,2.4,  !- X,Y,Z ==> Vertex 1 {m}",
		"    3.7,11.6,2.4,  !- X,Y,Z ==> Vertex 2 {m}",
		"    0.0,15.2,2.4,  !- X,Y,Z ==> Vertex 3 {m}",
		"    30.5,15.2,2.4;  !- X,Y,Z ==> Vertex 4 {m}",

		"  BuildingSurface:Detailed,",
		"    C4-1P,                   !- Name",
		"    FLOOR,                   !- Surface Type",
		"    reverseCeiling with Radiant,  !- Construction Name",
		"    PLENUM-1,                !- Zone Name",
		"    Surface,                 !- Outside Boundary Condition",
		"    C4-1,                    !- Outside Boundary Condition Object",
		"    NoSun,                   !- Sun Exposure",
		"    NoWind,                  !- Wind Exposure",
		"    0.0,                     !- View Factor to Ground",
		"    4,                       !- Number of Vertices",
		"    3.7,3.7,2.4,  !- X,Y,Z ==> Vertex 1 {m}",
		"    0.0,0.0,2.4,  !- X,Y,Z ==> Vertex 2 {m}",
		"    0.0,15.2,2.4,  !- X,Y,Z ==> Vertex 3 {m}",
		"    3.7,11.6,2.4;  !- X,Y,Z ==> Vertex 4 {m}",

		"  BuildingSurface:Detailed,",
		"    C5-1P,                   !- Name",
		"    FLOOR,                   !- Surface Type",
		"    reverseceiling with Radiant,  !- Construction Name",
		"    PLENUM-1,                !- Zone Name",
		"    Surface,                 !- Outside Boundary Condition",
		"    C5-1,                    !- Outside Boundary Condition Object",
		"    NoSun,                   !- Sun Exposure",
		"    NoWind,                  !- Wind Exposure",
		"    0.0,                     !- View Factor to Ground",
		"    4,                       !- Number of Vertices",
		"    26.8,11.6,2.4,  !- X,Y,Z ==> Vertex 1 {m}",
		"    26.8,3.7,2.4,  !- X,Y,Z ==> Vertex 2 {m}",
		"    3.7,3.7,2.4,  !- X,Y,Z ==> Vertex 3 {m}",
		"    3.7,11.6,2.4;  !- X,Y,Z ==> Vertex 4 {m}",

		"  BuildingSurface:Detailed,",
		"    FRONT-1,                 !- Name",
		"    WALL,                    !- Surface Type",
		"    WALL-1,                  !- Construction Name",
		"    SPACE1-1,                !- Zone Name",
		"    Outdoors,                !- Outside Boundary Condition",
		"    ,                        !- Outside Boundary Condition Object",
		"    SunExposed,              !- Sun Exposure",
		"    WindExposed,             !- Wind Exposure",
		"    0.50000,                 !- View Factor to Ground",
		"    4,                       !- Number of Vertices",
		"    0.0,0.0,2.4,  !- X,Y,Z ==> Vertex 1 {m}",
		"    0.0,0.0,0.0,  !- X,Y,Z ==> Vertex 2 {m}",
		"    30.5,0.0,0.0,  !- X,Y,Z ==> Vertex 3 {m}",
		"    30.5,0.0,2.4;  !- X,Y,Z ==> Vertex 4 {m}",

		"  BuildingSurface:Detailed,",
		"    C1-1,                    !- Name",
		"    CEILING,                 !- Surface Type",
		"    ceiling with Radiant,    !- Construction Name",
		"    SPACE1-1,                !- Zone Name",
		"    Surface,                 !- Outside Boundary Condition",
		"    C1-1P,                   !- Outside Boundary Condition Object",
		"    NoSun,                   !- Sun Exposure",
		"    NoWind,                  !- Wind Exposure",
		"    0.0,                     !- View Factor to Ground",
		"    4,                       !- Number of Vertices",
		"    3.7,3.7,2.4,  !- X,Y,Z ==> Vertex 1 {m}",
		"    0.0,0.0,2.4,  !- X,Y,Z ==> Vertex 2 {m}",
		"    30.5,0.0,2.4,  !- X,Y,Z ==> Vertex 3 {m}",
		"    26.8,3.7,2.4;  !- X,Y,Z ==> Vertex 4 {m}",

		"  BuildingSurface:Detailed,",
		"    F1-1,                    !- Name",
		"    FLOOR,                   !- Surface Type",
		"    FLOOR-1,                 !- Construction Name",
		"    SPACE1-1,                !- Zone Name",
		"    Ground,                  !- Outside Boundary Condition",
		"    ,                        !- Outside Boundary Condition Object",
		"    NoSun,                   !- Sun Exposure",
		"    NoWind,                  !- Wind Exposure",
		"    0.0,                     !- View Factor to Ground",
		"    4,                       !- Number of Vertices",
		"    26.8,3.7,0.0,  !- X,Y,Z ==> Vertex 1 {m}",
		"    30.5,0.0,0.0,  !- X,Y,Z ==> Vertex 2 {m}",
		"    0.0,0.0,0.0,  !- X,Y,Z ==> Vertex 3 {m}",
		"    3.7,3.7,0.0;  !- X,Y,Z ==> Vertex 4 {m}",

		"  BuildingSurface:Detailed,",
		"    SB12,                    !- Name",
		"    WALL,                    !- Surface Type",
		"    INT-WALL-1,              !- Construction Name",
		"    SPACE1-1,                !- Zone Name",
		"    Surface,                 !- Outside Boundary Condition",
		"    SB21,                    !- Outside Boundary Condition Object",
		"    NoSun,                   !- Sun Exposure",
		"    NoWind,                  !- Wind Exposure",
		"    0.0,                     !- View Factor to Ground",
		"    4,                       !- Number of Vertices",
		"    30.5,0.0,2.4,  !- X,Y,Z ==> Vertex 1 {m}",
		"    30.5,0.0,0.0,  !- X,Y,Z ==> Vertex 2 {m}",
		"    26.8,3.7,0.0,  !- X,Y,Z ==> Vertex 3 {m}",
		"    26.8,3.7,2.4;  !- X,Y,Z ==> Vertex 4 {m}",

		"  BuildingSurface:Detailed,",
		"    SB14,                    !- Name",
		"    WALL,                    !- Surface Type",
		"    INT-WALL-1,              !- Construction Name",
		"    SPACE1-1,                !- Zone Name",
		"    Surface,                 !- Outside Boundary Condition",
		"    SB41,                    !- Outside Boundary Condition Object",
		"    NoSun,                   !- Sun Exposure",
		"    NoWind,                  !- Wind Exposure",
		"    0.0,                     !- View Factor to Ground",
		"    4,                       !- Number of Vertices",
		"    3.7,3.7,2.4,  !- X,Y,Z ==> Vertex 1 {m}",
		"    3.7,3.7,0.0,  !- X,Y,Z ==> Vertex 2 {m}",
		"    0.0,0.0,0.0,  !- X,Y,Z ==> Vertex 3 {m}",
		"    0.0,0.0,2.4;  !- X,Y,Z ==> Vertex 4 {m}",

		"  BuildingSurface:Detailed,",
		"    SB15,                    !- Name",
		"    WALL,                    !- Surface Type",
		"    INT-WALL-1,              !- Construction Name",
		"    SPACE1-1,                !- Zone Name",
		"    Surface,                 !- Outside Boundary Condition",
		"    SB51,                    !- Outside Boundary Condition Object",
		"    NoSun,                   !- Sun Exposure",
		"    NoWind,                  !- Wind Exposure",
		"    0.0,                     !- View Factor to Ground",
		"    4,                       !- Number of Vertices",
		"    26.8,3.7,2.4,  !- X,Y,Z ==> Vertex 1 {m}",
		"    26.8,3.7,0.0,  !- X,Y,Z ==> Vertex 2 {m}",
		"    3.7,3.7,0.0,  !- X,Y,Z ==> Vertex 3 {m}",
		"    3.7,3.7,2.4;  !- X,Y,Z ==> Vertex 4 {m}",

		"  BuildingSurface:Detailed,",
		"    RIGHT-1,                 !- Name",
		"    WALL,                    !- Surface Type",
		"    WALL-1,                  !- Construction Name",
		"    SPACE2-1,                !- Zone Name",
		"    Outdoors,                !- Outside Boundary Condition",
		"    ,                        !- Outside Boundary Condition Object",
		"    SunExposed,              !- Sun Exposure",
		"    WindExposed,             !- Wind Exposure",
		"    0.50000,                 !- View Factor to Ground",
		"    4,                       !- Number of Vertices",
		"    30.5,0.0,2.4,  !- X,Y,Z ==> Vertex 1 {m}",
		"    30.5,0.0,0.0,  !- X,Y,Z ==> Vertex 2 {m}",
		"    30.5,15.2,0.0,  !- X,Y,Z ==> Vertex 3 {m}",
		"    30.5,15.2,2.4;  !- X,Y,Z ==> Vertex 4 {m}",

		"  BuildingSurface:Detailed,",
		"    C2-1,                    !- Name",
		"    CEILING,                 !- Surface Type",
		"    Ceiling with Radiant,    !- Construction Name",
		"    SPACE2-1,                !- Zone Name",
		"    Surface,                 !- Outside Boundary Condition",
		"    C2-1P,                   !- Outside Boundary Condition Object",
		"    NoSun,                   !- Sun Exposure",
		"    NoWind,                  !- Wind Exposure",
		"    0.0,                     !- View Factor to Ground",
		"    4,                       !- Number of Vertices",
		"    26.8,3.7,2.4,  !- X,Y,Z ==> Vertex 1 {m}",
		"    30.5,0.0,2.4,  !- X,Y,Z ==> Vertex 2 {m}",
		"    30.5,15.2,2.4,  !- X,Y,Z ==> Vertex 3 {m}",
		"    26.8,11.6,2.4;  !- X,Y,Z ==> Vertex 4 {m}",

		"  BuildingSurface:Detailed,",
		"    F2-1,                    !- Name",
		"    FLOOR,                   !- Surface Type",
		"    Floor with radiant,      !- Construction Name",
		"    SPACE2-1,                !- Zone Name",
		"    Ground,                  !- Outside Boundary Condition",
		"    ,                        !- Outside Boundary Condition Object",
		"    NoSun,                   !- Sun Exposure",
		"    NoWind,                  !- Wind Exposure",
		"    0.0,                     !- View Factor to Ground",
		"    4,                       !- Number of Vertices",
		"    26.8,11.6,0.0,  !- X,Y,Z ==> Vertex 1 {m}",
		"    30.5,15.2,0.0,  !- X,Y,Z ==> Vertex 2 {m}",
		"    30.5,0.0,0.0,  !- X,Y,Z ==> Vertex 3 {m}",
		"    26.8,3.7,0.0;  !- X,Y,Z ==> Vertex 4 {m}",

		"  BuildingSurface:Detailed,",
		"    SB21,                    !- Name",
		"    WALL,                    !- Surface Type",
		"    INT-WALL-1,              !- Construction Name",
		"    SPACE2-1,                !- Zone Name",
		"    Surface,                 !- Outside Boundary Condition",
		"    SB12,                    !- Outside Boundary Condition Object",
		"    NoSun,                   !- Sun Exposure",
		"    NoWind,                  !- Wind Exposure",
		"    0.0,                     !- View Factor to Ground",
		"    4,                       !- Number of Vertices",
		"    26.8,3.7,2.4,  !- X,Y,Z ==> Vertex 1 {m}",
		"    26.8,3.7,0.0,  !- X,Y,Z ==> Vertex 2 {m}",
		"    30.5,0.0,0.0,  !- X,Y,Z ==> Vertex 3 {m}",
		"    30.5,0.0,2.4;  !- X,Y,Z ==> Vertex 4 {m}",

		"  BuildingSurface:Detailed,",
		"    SB23,                    !- Name",
		"    WALL,                    !- Surface Type",
		"    INT-WALL-1,              !- Construction Name",
		"    SPACE2-1,                !- Zone Name",
		"    Surface,                 !- Outside Boundary Condition",
		"    SB32,                    !- Outside Boundary Condition Object",
		"    NoSun,                   !- Sun Exposure",
		"    NoWind,                  !- Wind Exposure",
		"    0.0,                     !- View Factor to Ground",
		"    4,                       !- Number of Vertices",
		"    30.5,15.2,2.4,  !- X,Y,Z ==> Vertex 1 {m}",
		"    30.5,15.2,0.0,  !- X,Y,Z ==> Vertex 2 {m}",
		"    26.8,11.6,0.0,  !- X,Y,Z ==> Vertex 3 {m}",
		"    26.8,11.6,2.4;  !- X,Y,Z ==> Vertex 4 {m}",

		"  BuildingSurface:Detailed,",
		"    SB25,                    !- Name",
		"    WALL,                    !- Surface Type",
		"    INT-WALL-1,              !- Construction Name",
		"    SPACE2-1,                !- Zone Name",
		"    Surface,                 !- Outside Boundary Condition",
		"    SB52,                    !- Outside Boundary Condition Object",
		"    NoSun,                   !- Sun Exposure",
		"    NoWind,                  !- Wind Exposure",
		"    0.0,                     !- View Factor to Ground",
		"    4,                       !- Number of Vertices",
		"    26.8,11.6,2.4,  !- X,Y,Z ==> Vertex 1 {m}",
		"    26.8,11.6,0.0,  !- X,Y,Z ==> Vertex 2 {m}",
		"    26.8,3.7,0.0,  !- X,Y,Z ==> Vertex 3 {m}",
		"    26.8,3.7,2.4;  !- X,Y,Z ==> Vertex 4 {m}",

		"  BuildingSurface:Detailed,",
		"    BACK-1,                  !- Name",
		"    WALL,                    !- Surface Type",
		"    WALL-1,                  !- Construction Name",
		"    SPACE3-1,                !- Zone Name",
		"    Outdoors,                !- Outside Boundary Condition",
		"    ,                        !- Outside Boundary Condition Object",
		"    SunExposed,              !- Sun Exposure",
		"    WindExposed,             !- Wind Exposure",
		"    0.50000,                 !- View Factor to Ground",
		"    4,                       !- Number of Vertices",
		"    30.5,15.2,2.4,  !- X,Y,Z ==> Vertex 1 {m}",
		"    30.5,15.2,0.0,  !- X,Y,Z ==> Vertex 2 {m}",
		"    0.0,15.2,0.0,  !- X,Y,Z ==> Vertex 3 {m}",
		"    0.0,15.2,2.4;  !- X,Y,Z ==> Vertex 4 {m}",

		"  BuildingSurface:Detailed,",
		"    C3-1,                    !- Name",
		"    CEILING,                 !- Surface Type",
		"    Ceiling with Radiant,    !- Construction Name",
		"    SPACE3-1,                !- Zone Name",
		"    Surface,                 !- Outside Boundary Condition",
		"    C3-1P,                   !- Outside Boundary Condition Object",
		"    NoSun,                   !- Sun Exposure",
		"    NoWind,                  !- Wind Exposure",
		"    0.0,                     !- View Factor to Ground",
		"    4,                       !- Number of Vertices",
		"    30.5,15.2,2.4,  !- X,Y,Z ==> Vertex 1 {m}",
		"    0.0,15.2,2.4,  !- X,Y,Z ==> Vertex 2 {m}",
		"    3.7,11.6,2.4,  !- X,Y,Z ==> Vertex 3 {m}",
		"    26.8,11.6,2.4;  !- X,Y,Z ==> Vertex 4 {m}",

		"  BuildingSurface:Detailed,",
		"    F3-1,                    !- Name",
		"    FLOOR,                   !- Surface Type",
		"    FLOOR-1,                 !- Construction Name",
		"    SPACE3-1,                !- Zone Name",
		"    Ground,                  !- Outside Boundary Condition",
		"    ,                        !- Outside Boundary Condition Object",
		"    NoSun,                   !- Sun Exposure",
		"    NoWind,                  !- Wind Exposure",
		"    0.0,                     !- View Factor to Ground",
		"    4,                       !- Number of Vertices",
		"    26.8,11.6,0.0,  !- X,Y,Z ==> Vertex 1 {m}",
		"    3.7,11.6,0.0,  !- X,Y,Z ==> Vertex 2 {m}",
		"    0.0,15.2,0.0,  !- X,Y,Z ==> Vertex 3 {m}",
		"    30.5,15.2,0.0;  !- X,Y,Z ==> Vertex 4 {m}",

		"  BuildingSurface:Detailed,",
		"    SB32,                    !- Name",
		"    WALL,                    !- Surface Type",
		"    INT-WALL-1,              !- Construction Name",
		"    SPACE3-1,                !- Zone Name",
		"    Surface,                 !- Outside Boundary Condition",
		"    SB23,                    !- Outside Boundary Condition Object",
		"    NoSun,                   !- Sun Exposure",
		"    NoWind,                  !- Wind Exposure",
		"    0.0,                     !- View Factor to Ground",
		"    4,                       !- Number of Vertices",
		"    26.8,11.6,2.4,  !- X,Y,Z ==> Vertex 1 {m}",
		"    26.8,11.6,0.0,  !- X,Y,Z ==> Vertex 2 {m}",
		"    30.5,15.2,0.0,  !- X,Y,Z ==> Vertex 3 {m}",
		"    30.5,15.2,2.4;  !- X,Y,Z ==> Vertex 4 {m}",

		"  BuildingSurface:Detailed,",
		"    SB34,                    !- Name",
		"    WALL,                    !- Surface Type",
		"    INT-WALL-1,              !- Construction Name",
		"    SPACE3-1,                !- Zone Name",
		"    Surface,                 !- Outside Boundary Condition",
		"    SB43,                    !- Outside Boundary Condition Object",
		"    NoSun,                   !- Sun Exposure",
		"    NoWind,                  !- Wind Exposure",
		"    0.0,                     !- View Factor to Ground",
		"    4,                       !- Number of Vertices",
		"    0.0,15.2,2.4,  !- X,Y,Z ==> Vertex 1 {m}",
		"    0.0,15.2,0.0,  !- X,Y,Z ==> Vertex 2 {m}",
		"    3.7,11.6,0.0,  !- X,Y,Z ==> Vertex 3 {m}",
		"    3.7,11.6,2.4;  !- X,Y,Z ==> Vertex 4 {m}",

		"  BuildingSurface:Detailed,",
		"    SB35,                    !- Name",
		"    WALL,                    !- Surface Type",
		"    INT-WALL-1,              !- Construction Name",
		"    SPACE3-1,                !- Zone Name",
		"    Surface,                 !- Outside Boundary Condition",
		"    SB53,                    !- Outside Boundary Condition Object",
		"    NoSun,                   !- Sun Exposure",
		"    NoWind,                  !- Wind Exposure",
		"    0.0,                     !- View Factor to Ground",
		"    4,                       !- Number of Vertices",
		"    3.7,11.6,2.4,  !- X,Y,Z ==> Vertex 1 {m}",
		"    3.7,11.6,0.0,  !- X,Y,Z ==> Vertex 2 {m}",
		"    26.8,11.6,0.0,  !- X,Y,Z ==> Vertex 3 {m}",
		"    26.8,11.6,2.4;  !- X,Y,Z ==> Vertex 4 {m}",

		"  BuildingSurface:Detailed,",
		"    LEFT-1,                  !- Name",
		"    WALL,                    !- Surface Type",
		"    WALL-1,                  !- Construction Name",
		"    SPACE4-1,                !- Zone Name",
		"    Outdoors,                !- Outside Boundary Condition",
		"    ,                        !- Outside Boundary Condition Object",
		"    SunExposed,              !- Sun Exposure",
		"    WindExposed,             !- Wind Exposure",
		"    0.50000,                 !- View Factor to Ground",
		"    4,                       !- Number of Vertices",
		"    0.0,15.2,2.4,  !- X,Y,Z ==> Vertex 1 {m}",
		"    0.0,15.2,0.0,  !- X,Y,Z ==> Vertex 2 {m}",
		"    0.0,0.0,0.0,  !- X,Y,Z ==> Vertex 3 {m}",
		"    0.0,0.0,2.4;  !- X,Y,Z ==> Vertex 4 {m}",

		"  BuildingSurface:Detailed,",
		"    C4-1,                    !- Name",
		"    CEILING,                 !- Surface Type",
		"    Ceiling with Radiant,    !- Construction Name",
		"    SPACE4-1,                !- Zone Name",
		"    Surface,                 !- Outside Boundary Condition",
		"    C4-1P,                   !- Outside Boundary Condition Object",
		"    NoSun,                   !- Sun Exposure",
		"    NoWind,                  !- Wind Exposure",
		"    0.0,                     !- View Factor to Ground",
		"    4,                       !- Number of Vertices",
		"    3.7,11.6,2.4,  !- X,Y,Z ==> Vertex 1 {m}",
		"    0.0,15.2,2.4,  !- X,Y,Z ==> Vertex 2 {m}",
		"    0.0,0.0,2.4,  !- X,Y,Z ==> Vertex 3 {m}",
		"    3.7,3.7,2.4;  !- X,Y,Z ==> Vertex 4 {m}",

		"  BuildingSurface:Detailed,",
		"    F4-1,                    !- Name",
		"    FLOOR,                   !- Surface Type",
		"    FLOOR-1,                 !- Construction Name",
		"    SPACE4-1,                !- Zone Name",
		"    Ground,                  !- Outside Boundary Condition",
		"    ,                        !- Outside Boundary Condition Object",
		"    NoSun,                   !- Sun Exposure",
		"    NoWind,                  !- Wind Exposure",
		"    0.0,                     !- View Factor to Ground",
		"    4,                       !- Number of Vertices",
		"    3.7,3.7,0.0,  !- X,Y,Z ==> Vertex 1 {m}",
		"    0.0,0.0,0.0,  !- X,Y,Z ==> Vertex 2 {m}",
		"    0.0,15.2,0.0,  !- X,Y,Z ==> Vertex 3 {m}",
		"    3.7,11.6,0.0;  !- X,Y,Z ==> Vertex 4 {m}",

		"  BuildingSurface:Detailed,",
		"    SB41,                    !- Name",
		"    WALL,                    !- Surface Type",
		"    INT-WALL-1,              !- Construction Name",
		"    SPACE4-1,                !- Zone Name",
		"    Surface,                 !- Outside Boundary Condition",
		"    SB14,                    !- Outside Boundary Condition Object",
		"    NoSun,                   !- Sun Exposure",
		"    NoWind,                  !- Wind Exposure",
		"    0.0,                     !- View Factor to Ground",
		"    4,                       !- Number of Vertices",
		"    0.0,0.0,2.4,  !- X,Y,Z ==> Vertex 1 {m}",
		"    0.0,0.0,0.0,  !- X,Y,Z ==> Vertex 2 {m}",
		"    3.7,3.7,0.0,  !- X,Y,Z ==> Vertex 3 {m}",
		"    3.7,3.7,2.4;  !- X,Y,Z ==> Vertex 4 {m}",

		"  BuildingSurface:Detailed,",
		"    SB43,                    !- Name",
		"    WALL,                    !- Surface Type",
		"    INT-WALL-1,              !- Construction Name",
		"    SPACE4-1,                !- Zone Name",
		"    Surface,                 !- Outside Boundary Condition",
		"    SB34,                    !- Outside Boundary Condition Object",
		"    NoSun,                   !- Sun Exposure",
		"    NoWind,                  !- Wind Exposure",
		"    0.0,                     !- View Factor to Ground",
		"    4,                       !- Number of Vertices",
		"    3.7,11.6,2.4,  !- X,Y,Z ==> Vertex 1 {m}",
		"    3.7,11.6,0.0,  !- X,Y,Z ==> Vertex 2 {m}",
		"    0.0,15.2,0.0,  !- X,Y,Z ==> Vertex 3 {m}",
		"    0.0,15.2,2.4;  !- X,Y,Z ==> Vertex 4 {m}",

		"  BuildingSurface:Detailed,",
		"    SB45,                    !- Name",
		"    WALL,                    !- Surface Type",
		"    INT-WALL-1,              !- Construction Name",
		"    SPACE4-1,                !- Zone Name",
		"    Surface,                 !- Outside Boundary Condition",
		"    SB54,                    !- Outside Boundary Condition Object",
		"    NoSun,                   !- Sun Exposure",
		"    NoWind,                  !- Wind Exposure",
		"    0.0,                     !- View Factor to Ground",
		"    4,                       !- Number of Vertices",
		"    3.7,3.7,2.4,  !- X,Y,Z ==> Vertex 1 {m}",
		"    3.7,3.7,0.0,  !- X,Y,Z ==> Vertex 2 {m}",
		"    3.7,11.6,0.0,  !- X,Y,Z ==> Vertex 3 {m}",
		"    3.7,11.6,2.4;  !- X,Y,Z ==> Vertex 4 {m}",

		"  BuildingSurface:Detailed,",
		"    C5-1,                    !- Name",
		"    CEILING,                 !- Surface Type",
		"    ceiling with Radiant,    !- Construction Name",
		"    SPACE5-1,                !- Zone Name",
		"    Surface,                 !- Outside Boundary Condition",
		"    C5-1P,                   !- Outside Boundary Condition Object",
		"    NoSun,                   !- Sun Exposure",
		"    NoWind,                  !- Wind Exposure",
		"    0.0,                     !- View Factor to Ground",
		"    4,                       !- Number of Vertices",
		"    3.7,11.6,2.4,  !- X,Y,Z ==> Vertex 1 {m}",
		"    3.7,3.7,2.4,  !- X,Y,Z ==> Vertex 2 {m}",
		"    26.8,3.7,2.4,  !- X,Y,Z ==> Vertex 3 {m}",
		"    26.8,11.6,2.4;  !- X,Y,Z ==> Vertex 4 {m}",

		"  BuildingSurface:Detailed,",
		"    F5-1,                    !- Name",
		"    FLOOR,                   !- Surface Type",
		"    FLOOR-1,                 !- Construction Name",
		"    SPACE5-1,                !- Zone Name",
		"    Ground,                  !- Outside Boundary Condition",
		"    ,                        !- Outside Boundary Condition Object",
		"    NoSun,                   !- Sun Exposure",
		"    NoWind,                  !- Wind Exposure",
		"    0.0,                     !- View Factor to Ground",
		"    4,                       !- Number of Vertices",
		"    26.8,11.6,0.0,  !- X,Y,Z ==> Vertex 1 {m}",
		"    26.8,3.7,0.0,  !- X,Y,Z ==> Vertex 2 {m}",
		"    3.7,3.7,0.0,  !- X,Y,Z ==> Vertex 3 {m}",
		"    3.7,11.6,0.0;  !- X,Y,Z ==> Vertex 4 {m}",

		"  BuildingSurface:Detailed,",
		"    SB51,                    !- Name",
		"    WALL,                    !- Surface Type",
		"    INT-WALL-1,              !- Construction Name",
		"    SPACE5-1,                !- Zone Name",
		"    Surface,                 !- Outside Boundary Condition",
		"    SB15,                    !- Outside Boundary Condition Object",
		"    NoSun,                   !- Sun Exposure",
		"    NoWind,                  !- Wind Exposure",
		"    0.0,                     !- View Factor to Ground",
		"    4,                       !- Number of Vertices",
		"    3.7,3.7,2.4,  !- X,Y,Z ==> Vertex 1 {m}",
		"    3.7,3.7,0.0,  !- X,Y,Z ==> Vertex 2 {m}",
		"    26.8,3.7,0.0,  !- X,Y,Z ==> Vertex 3 {m}",
		"    26.8,3.7,2.4;  !- X,Y,Z ==> Vertex 4 {m}",

		"  BuildingSurface:Detailed,",
		"    SB52,                    !- Name",
		"    WALL,                    !- Surface Type",
		"    INT-WALL-1,              !- Construction Name",
		"    SPACE5-1,                !- Zone Name",
		"    Surface,                 !- Outside Boundary Condition",
		"    SB25,                    !- Outside Boundary Condition Object",
		"    NoSun,                   !- Sun Exposure",
		"    NoWind,                  !- Wind Exposure",
		"    0.0,                     !- View Factor to Ground",
		"    4,                       !- Number of Vertices",
		"    26.8,3.7,2.4,  !- X,Y,Z ==> Vertex 1 {m}",
		"    26.8,3.7,0.0,  !- X,Y,Z ==> Vertex 2 {m}",
		"    26.8,11.6,0.0,  !- X,Y,Z ==> Vertex 3 {m}",
		"    26.8,11.6,2.4;  !- X,Y,Z ==> Vertex 4 {m}",

		"  BuildingSurface:Detailed,",
		"    SB53,                    !- Name",
		"    WALL,                    !- Surface Type",
		"    INT-WALL-1,              !- Construction Name",
		"    SPACE5-1,                !- Zone Name",
		"    Surface,                 !- Outside Boundary Condition",
		"    SB35,                    !- Outside Boundary Condition Object",
		"    NoSun,                   !- Sun Exposure",
		"    NoWind,                  !- Wind Exposure",
		"    0.0,                     !- View Factor to Ground",
		"    4,                       !- Number of Vertices",
		"    26.8,11.6,2.4,  !- X,Y,Z ==> Vertex 1 {m}",
		"    26.8,11.6,0.0,  !- X,Y,Z ==> Vertex 2 {m}",
		"    3.7,11.6,0.0,  !- X,Y,Z ==> Vertex 3 {m}",
		"    3.7,11.6,2.4;  !- X,Y,Z ==> Vertex 4 {m}",

		"  BuildingSurface:Detailed,",
		"    SB54,                    !- Name",
		"    WALL,                    !- Surface Type",
		"    INT-WALL-1,              !- Construction Name",
		"    SPACE5-1,                !- Zone Name",
		"    Surface,                 !- Outside Boundary Condition",
		"    SB45,                    !- Outside Boundary Condition Object",
		"    NoSun,                   !- Sun Exposure",
		"    NoWind,                  !- Wind Exposure",
		"    0.0,                     !- View Factor to Ground",
		"    4,                       !- Number of Vertices",
		"    3.7,11.6,2.4,  !- X,Y,Z ==> Vertex 1 {m}",
		"    3.7,11.6,0.0,  !- X,Y,Z ==> Vertex 2 {m}",
		"    3.7,3.7,0.0,  !- X,Y,Z ==> Vertex 3 {m}",
		"    3.7,3.7,2.4;  !- X,Y,Z ==> Vertex 4 {m}",


		"  FenestrationSurface:Detailed,",
		"    WF-1,                    !- Name",
		"    WINDOW,                  !- Surface Type",
		"    Dbl Clr 3mm/13mm Air,    !- Construction Name",
		"    FRONT-1,                 !- Building Surface Name",
		"    ,                        !- Outside Boundary Condition Object",
		"    0.50000,                 !- View Factor to Ground",
		"    ,                        !- Shading Control Name",
		"    ,                        !- Frame and Divider Name",
		"    1,                       !- Multiplier",
		"    4,                       !- Number of Vertices",
		"    3.0,0.0,2.1,  !- X,Y,Z ==> Vertex 1 {m}",
		"    3.0,0.0,0.9,  !- X,Y,Z ==> Vertex 2 {m}",
		"    16.8,0.0,0.9,  !- X,Y,Z ==> Vertex 3 {m}",
		"    16.8,0.0,2.1;  !- X,Y,Z ==> Vertex 4 {m}",

		"  FenestrationSurface:Detailed,",
		"    DF-1,                    !- Name",
		"    GLASSDOOR,               !- Surface Type",
		"    Sgl Grey 3mm,            !- Construction Name",
		"    FRONT-1,                 !- Building Surface Name",
		"    ,                        !- Outside Boundary Condition Object",
		"    0.50000,                 !- View Factor to Ground",
		"    ,                        !- Shading Control Name",
		"    ,                        !- Frame and Divider Name",
		"    1,                       !- Multiplier",
		"    4,                       !- Number of Vertices",
		"    21.3,0.0,2.1,  !- X,Y,Z ==> Vertex 1 {m}",
		"    21.3,0.0,0.0,  !- X,Y,Z ==> Vertex 2 {m}",
		"    23.8,0.0,0.0,  !- X,Y,Z ==> Vertex 3 {m}",
		"    23.8,0.0,2.1;  !- X,Y,Z ==> Vertex 4 {m}",

		"  FenestrationSurface:Detailed,",
		"    WR-1,                    !- Name",
		"    WINDOW,                  !- Surface Type",
		"    Dbl Clr 3mm/13mm Air,    !- Construction Name",
		"    RIGHT-1,                 !- Building Surface Name",
		"    ,                        !- Outside Boundary Condition Object",
		"    0.50000,                 !- View Factor to Ground",
		"    ,                        !- Shading Control Name",
		"    ,                        !- Frame and Divider Name",
		"    1,                       !- Multiplier",
		"    4,                       !- Number of Vertices",
		"    30.5,3.8,2.1,  !- X,Y,Z ==> Vertex 1 {m}",
		"    30.5,3.8,0.9,  !- X,Y,Z ==> Vertex 2 {m}",
		"    30.5,11.4,0.9,  !- X,Y,Z ==> Vertex 3 {m}",
		"    30.5,11.4,2.1;  !- X,Y,Z ==> Vertex 4 {m}",

		"  FenestrationSurface:Detailed,",
		"    WB-1,                    !- Name",
		"    WINDOW,                  !- Surface Type",
		"    Dbl Clr 3mm/13mm Air,    !- Construction Name",
		"    BACK-1,                  !- Building Surface Name",
		"    ,                        !- Outside Boundary Condition Object",
		"    0.50000,                 !- View Factor to Ground",
		"    ,                        !- Shading Control Name",
		"    ,                        !- Frame and Divider Name",
		"    1,                       !- Multiplier",
		"    4,                       !- Number of Vertices",
		"    27.4,15.2,2.1,  !- X,Y,Z ==> Vertex 1 {m}",
		"    27.4,15.2,0.9,  !- X,Y,Z ==> Vertex 2 {m}",
		"    13.7,15.2,0.9,  !- X,Y,Z ==> Vertex 3 {m}",
		"    13.7,15.2,2.1;  !- X,Y,Z ==> Vertex 4 {m}",

		"  FenestrationSurface:Detailed,",
		"    DB-1,                    !- Name",
		"    GLASSDOOR,               !- Surface Type",
		"    Sgl Grey 3mm,            !- Construction Name",
		"    BACK-1,                  !- Building Surface Name",
		"    ,                        !- Outside Boundary Condition Object",
		"    0.50000,                 !- View Factor to Ground",
		"    ,                        !- Shading Control Name",
		"    ,                        !- Frame and Divider Name",
		"    1,                       !- Multiplier",
		"    4,                       !- Number of Vertices",
		"    9.1,15.2,2.1,  !- X,Y,Z ==> Vertex 1 {m}",
		"    9.1,15.2,0.0,  !- X,Y,Z ==> Vertex 2 {m}",
		"    7.0,15.2,0.0,  !- X,Y,Z ==> Vertex 3 {m}",
		"    7.0,15.2,2.1;  !- X,Y,Z ==> Vertex 4 {m}",

		"  FenestrationSurface:Detailed,",
		"    WL-1,                    !- Name",
		"    WINDOW,                  !- Surface Type",
		"    Dbl Clr 3mm/13mm Air,    !- Construction Name",
		"    LEFT-1,                  !- Building Surface Name",
		"    ,                        !- Outside Boundary Condition Object",
		"    0.50000,                 !- View Factor to Ground",
		"    ,                        !- Shading Control Name",
		"    ,                        !- Frame and Divider Name",
		"    1,                       !- Multiplier",
		"    4,                       !- Number of Vertices",
		"    0.0,11.4,2.1,  !- X,Y,Z ==> Vertex 1 {m}",
		"    0.0,11.4,0.9,  !- X,Y,Z ==> Vertex 2 {m}",
		"    0.0,3.8,0.9,  !- X,Y,Z ==> Vertex 3 {m}",
		"    0.0,3.8,2.1;  !- X,Y,Z ==> Vertex 4 {m}",


		"  Shading:Zone:Detailed,",
		"    Main South Overhang,     !- Name",
		"    FRONT-1,                 !- Base Surface Name",
		"    ShadeTransSch,           !- Transmittance Schedule Name",
		"    4,                       !- Number of Vertices",
		"    0.0,-1.3,2.2,  !- X,Y,Z ==> Vertex 1 {m}",
		"    0.0,0.0,2.2,  !- X,Y,Z ==> Vertex 2 {m}",
		"    19.8,0.0,2.2,  !- X,Y,Z ==> Vertex 3 {m}",
		"    19.8,-1.3,2.2;  !- X,Y,Z ==> Vertex 4 {m}",

		"  Shading:Zone:Detailed,",
		"    South Door Overhang,     !- Name",
		"    FRONT-1,                 !- Base Surface Name",
		"    ShadeTransSch,           !- Transmittance Schedule Name",
		"    4,                       !- Number of Vertices",
		"    21.0,-2.0,2.6,  !- X,Y,Z ==> Vertex 1 {m}",
		"    21.0,0.0,2.6,  !- X,Y,Z ==> Vertex 2 {m}",
		"    24.1,0.0,2.6,  !- X,Y,Z ==> Vertex 3 {m}",
		"    24.1,-2.0,2.6;  !- X,Y,Z ==> Vertex 4 {m}",


		"  ZoneHVAC:VentilatedSlab,",
		"    Zone1VentSlab,           !- Name",
		"    FanAndCoilAvailSched,    !- Availability Schedule Name",
		"    SPACE1-1,                !- Zone Name",
		"    Z125,                    !- Surface Name or Radiant Surface Group Name",
		"    0.84,                    !- Maximum Air Flow Rate {m3/s}",
		"    VariablePercent,         !- Outdoor Air Control Type",
		"    0.168,                   !- Minimum Outdoor Air Flow Rate {m3/s}",
		"    FanAndCoilAvailSched,    !- Minimum Outdoor Air Schedule Name",
		"    0.84,                    !- Maximum Outdoor Air Flow Rate {m3/s}",
		"    VentSlabMaxOA,           !- Maximum Outdoor Air Fraction or Temperature Schedule Name",
		"    SeriesSlabs,             !- System Configuration Type",
		"    0,                       !- Hollow Core Inside Diameter {m}",
		"    0,                       !- Hollow Core Length {m}",
		"    0,                       !- Number of Cores",
		"    MeanAirTemperature,      !- Temperature Control Type",
		"    VentSlabHotHighAir,      !- Heating High Air Temperature Schedule Name",
		"    VentSlabHotLowAir,       !- Heating Low Air Temperature Schedule Name",
		"    VentSlabHotHighControl,  !- Heating High Control Temperature Schedule Name",
		"    VentSlabHotLowControl,   !- Heating Low Control Temperature Schedule Name",
		"    VentSlabCoolHighAir,     !- Cooling High Air Temperature Schedule Name",
		"    VentSlabCoolLowAir,      !- Cooling Low Air Temperature Schedule Name",
		"    VentSlabCoolHighControl, !- Cooling High Control Temperature Schedule Name",
		"    VentSlabCoolLowControl,  !- Cooling Low Control Temperature Schedule Name",
		"    Zone1VentSlabReturnAirNode,  !- Return Air Node Name",
		"    Zone1VentslabSlabInNode, !- Slab In Node Name",
		"    ,                        !- Zone Supply Air Node Name",
		"    Zone1VentSlabOAInNode,   !- Outdoor Air Node Name",
		"    Zone1VentSlabExhNode,    !- Relief Air Node Name",
		"    Zone1VentSlabOAMixerOutletNode,  !- Outdoor Air Mixer Outlet Node Name",
		"    Zone1VentSlabFanOutletNode,  !- Fan Outlet Node Name",
		"    Zone1VentSlabFan,        !- Fan Name",
		"    HeatingAndCooling,       !- Coil Option Type",
		"    Coil:Heating:Water,      !- Heating Coil Object Type",
		"    Zone1VentSlabHeatingCoil,!- Heating Coil Name",
		"    Zone1VentSlabHwInLetNode,!- Hot Water or Steam Inlet Node Name",
		"    Coil:Cooling:Water,      !- Cooling Coil Object Type",
		"    Zone1VentSlabCoolingCoil,!- Cooling Coil Name",
		"    Zone1VentSlabChWInletNode;  !- Cold Water Inlet Node Name",

		"  ZoneHVAC:VentilatedSlab,",
		"    Zone4VentSlab,           !- Name",
		"    FanAndCoilAvailSched,    !- Availability Schedule Name",
		"    SPACE4-1,                !- Zone Name",
		"    Z43,                     !- Surface Name or Radiant Surface Group Name",
		"    0.84,                    !- Maximum Air Flow Rate {m3/s}",
		"    VariablePercent,         !- Outdoor Air Control Type",
		"    0.168,                   !- Minimum Outdoor Air Flow Rate {m3/s}",
		"    FanAndCoilAvailSched,    !- Minimum Outdoor Air Schedule Name",
		"    0.84,                    !- Maximum Outdoor Air Flow Rate {m3/s}",
		"    VentSlabMaxOA,           !- Maximum Outdoor Air Fraction or Temperature Schedule Name",
		"    SeriesSlabs,             !- System Configuration Type",
		"    0,                       !- Hollow Core Inside Diameter {m}",
		"    0,                       !- Hollow Core Length {m}",
		"    0,                       !- Number of Cores",
		"    OutdoorDryBulbTemperature,  !- Temperature Control Type",
		"    VentSlabHotHighAir,      !- Heating High Air Temperature Schedule Name",
		"    VentSlabHotLowAir,       !- Heating Low Air Temperature Schedule Name",
		"    VentSlabHotHighControl,  !- Heating High Control Temperature Schedule Name",
		"    VentSlabHotLowControl,   !- Heating Low Control Temperature Schedule Name",
		"    VentSlabCoolHighAir,     !- Cooling High Air Temperature Schedule Name",
		"    VentSlabCoolLowAir,      !- Cooling Low Air Temperature Schedule Name",
		"    VentSlabCoolHighControl, !- Cooling High Control Temperature Schedule Name",
		"    VentSlabCoolLowControl2, !- Cooling Low Control Temperature Schedule Name",
		"    Zone4VentSlabReturnAirNode,  !- Return Air Node Name",
		"    Zone4VentslabSlabInNode, !- Slab In Node Name",
		"    ,                        !- Zone Supply Air Node Name",
		"    Zone4VentSlabOAInNode,   !- Outdoor Air Node Name",
		"    Zone4VentSlabExhNode,    !- Relief Air Node Name",
		"    Zone4VentSlabOAMixerOutletNode,  !- Outdoor Air Mixer Outlet Node Name",
		"    Zone4VentSlabFanOutletNode,  !- Fan Outlet Node Name",
		"    Zone4VentSlabFan,        !- Fan Name",
		"    HeatingAndCooling,       !- Coil Option Type",
		"    Coil:Heating:Electric,   !- Heating Coil Object Type",
		"    Zone4VentSlabHeatingCoil,!- Heating Coil Name",
		"    ,                        !- Hot Water or Steam Inlet Node Name",
		"    Coil:Cooling:Water,      !- Cooling Coil Object Type",
		"    Zone4VentSlabCoolingCoil,!- Cooling Coil Name",
		"    Zone4VentSlabChWInletNode;  !- Cold Water Inlet Node Name",


		"  ZoneHVAC:VentilatedSlab:SlabGroup,",
		"    Z125,                    !- Name",
		"    SPACE1-1,                !- Zone 1 Name",
		"    C1-1,                    !- Surface 1 Name",
		"    0.05,                    !- Core Diameter for Surface 1 {m}",
		"    30,                      !- Core Length for Surface 1 {m}",
		"    20,                      !- Core Numbers for Surface 1",
		"    Z1VentslabIn,            !- Slab Inlet Node Name for Surface 1",
		"    Z1VentSlabout,           !- Slab Outlet Node Name for Surface 1",
		"    SPACE2-1,                !- Zone 2 Name",
		"    C2-1,                    !- Surface 2 Name",
		"    0.05,                    !- Core Diameter for Surface 2 {m}",
		"    15,                      !- Core Length for Surface 2 {m}",
		"    20,                      !- Core Numbers for Surface 2",
		"    Z2VentSlabIn,            !- Slab Inlet Node Name for Surface 2",
		"    Z2VentSlabOut,           !- Slab Outlet Node Name for Surface 2",
		"    SPACE5-1,                !- Zone 3 Name",
		"    C5-1,                    !- Surface 3 Name",
		"    0.05,                    !- Core Diameter for Surface 3 {m}",
		"    30,                      !- Core Length for Surface 3 {m}",
		"    20,                      !- Core Numbers for Surface 3",
		"    Z5VentSlabIn,            !- Slab Inlet Node Name for Surface 3",
		"    Z5VentSlabOut;           !- Slab Outlet Node Name for Surface 3",

		"  ZoneHVAC:VentilatedSlab:SlabGroup,",
		"    Z43,                     !- Name",
		"    SPACE4-1,                !- Zone 1 Name",
		"    C4-1,                    !- Surface 1 Name",
		"    0.05,                    !- Core Diameter for Surface 1 {m}",
		"    30,                      !- Core Length for Surface 1 {m}",
		"    20,                      !- Core Numbers for Surface 1",
		"    Z4VentSlabIn,            !- Slab Inlet Node Name for Surface 1",
		"    Z4VentSlabOut,           !- Slab Outlet Node Name for Surface 1",
		"    SPACE3-1,                !- Zone 2 Name",
		"    C3-1,                    !- Surface 2 Name",
		"    0.05,                    !- Core Diameter for Surface 2 {m}",
		"    30,                      !- Core Length for Surface 2 {m}",
		"    20,                      !- Core Numbers for Surface 2",
		"    Z3VentSlabIn,            !- Slab Inlet Node Name for Surface 2",
		"    Z3VentSlabOut;           !- Slab Outlet Node Name for Surface 2",


		"  ZoneHVAC:EquipmentList,",
		"    Zone1Equipment,          !- Name",
		"    ZoneHVAC:VentilatedSlab, !- Zone Equipment 1 Object Type",
		"    Zone1VentSlab,           !- Zone Equipment 1 Name",
		"    1,                       !- Zone Equipment 1 Cooling Sequence",
		"    1;                       !- Zone Equipment 1 Heating or No-Load Sequence",

		"  ZoneHVAC:EquipmentList,",
		"    Zone4Equipment,          !- Name",
		"    ZoneHVAC:VentilatedSlab, !- Zone Equipment 1 Object Type",
		"    Zone4VentSlab,           !- Zone Equipment 1 Name",
		"    1,                       !- Zone Equipment 1 Cooling Sequence",
		"    1;                       !- Zone Equipment 1 Heating or No-Load Sequence",


		"  ZoneHVAC:EquipmentConnections,",
		"    SPACE1-1,                !- Zone Name",
		"    Zone1Equipment,          !- Zone Conditioning Equipment List Name",
		"    ,                        !- Zone Air Inlet Node or NodeList Name",
		"    ,                        !- Zone Air Exhaust Node or NodeList Name",
		"    Zone 1 Node,             !- Zone Air Node Name",
		"    Zone 1 Outlet Node;      !- Zone Return Air Node Name",

		"  ZoneHVAC:EquipmentConnections,",
		"    SPACE4-1,                !- Zone Name",
		"    Zone4Equipment,          !- Zone Conditioning Equipment List Name",
		"    ,                        !- Zone Air Inlet Node or NodeList Name",
		"    ,                        !- Zone Air Exhaust Node or NodeList Name",
		"    Zone 4 Node,             !- Zone Air Node Name",
		"    Zone 4 Outlet Node;      !- Zone Return Air Node Name",


		"  Fan:ConstantVolume,",
		"    Zone1VentSlabFan,        !- Name",
		"    FanAndCoilAvailSched,    !- Availability Schedule Name",
		"    0.5,                     !- Fan Total Efficiency",
		"    75.0,                    !- Pressure Rise {Pa}",
		"    0.84,                    !- Maximum Flow Rate {m3/s}",
		"    0.9,                     !- Motor Efficiency",
		"    1.0,                     !- Motor In Airstream Fraction",
		"    Zone1VentSlabOAMixerOutletNode,  !- Air Inlet Node Name",
		"    Zone1VentSlabFanOutletNode;  !- Air Outlet Node Name",

		"  Fan:ConstantVolume,",
		"    Zone4VentSlabFan,        !- Name",
		"    FanAndCoilAvailSched,    !- Availability Schedule Name",
		"    0.5,                     !- Fan Total Efficiency",
		"    75.0,                    !- Pressure Rise {Pa}",
		"    0.84,                    !- Maximum Flow Rate {m3/s}",
		"    0.9,                     !- Motor Efficiency",
		"    1.0,                     !- Motor In Airstream Fraction",
		"    Zone4VentSlabOAMixerOutletNode,  !- Air Inlet Node Name",
		"    Zone4VentSlabFanOutletNode;  !- Air Outlet Node Name",


		"  Coil:Cooling:Water,",
		"    Zone1VentSlabCoolingCoil,!- Name",
		"    FanAndCoilAvailSched,   !- Availability Schedule Name",
		"    0.0010,                  !- Design Water Flow Rate {m3/s}",
		"    0.84,                    !- Design Air Flow Rate {m3/s}",
		"    6.67,                    !- Design Inlet Water Temperature {C}",
		"    35,                      !- Design Inlet Air Temperature {C}",
		"    13.23,                   !- Design Outlet Air Temperature {C}",
		"    0.013,                   !- Design Inlet Air Humidity Ratio {kgWater/kgDryAir}",
		"    0.0087138,               !- Design Outlet Air Humidity Ratio {kgWater/kgDryAir}",
		"    Zone1VentSlabChWInletNode,  !- Water Inlet Node Name",
		"    Zone1VentSlabChWOutletNode,  !- Water Outlet Node Name",
		"    Zone1VentSlabFanOutletNode,  !- Air Inlet Node Name",
		"    Zone1VentSlabCCoutletNode,  !- Air Outlet Node Name",
		"    SimpleAnalysis,          !- Type of Analysis",
		"    CrossFlow;               !- Heat Exchanger Configuration",

		"  Coil:Cooling:Water,",
		"    Zone4VentSlabCoolingCoil,!- Name",
		"    FanAndCoilAvailSched,   !- Availability Schedule Name",
		"    0.0010,                  !- Design Water Flow Rate {m3/s}",
		"    0.84,                    !- Design Air Flow Rate {m3/s}",
		"    6.67,                    !- Design Inlet Water Temperature {C}",
		"    35,                      !- Design Inlet Air Temperature {C}",
		"    13.23,                   !- Design Outlet Air Temperature {C}",
		"    0.013,                   !- Design Inlet Air Humidity Ratio {kgWater/kgDryAir}",
		"    0.0087138,               !- Design Outlet Air Humidity Ratio {kgWater/kgDryAir}",
		"    Zone4VentSlabChWInletNode,  !- Water Inlet Node Name",
		"    Zone4VentSlabChWOutletNode,  !- Water Outlet Node Name",
		"    Zone4VentSlabFanOutletNode,  !- Air Inlet Node Name",
		"    Zone4VentSlabCCoutletNode,  !- Air Outlet Node Name",
		"    SimpleAnalysis,          !- Type of Analysis",
		"    CrossFlow;               !- Heat Exchanger Configuration",


		"  Coil:Heating:Water,",
		"    Zone1VentSlabHeatingCoil,!- Name",
		"    FanAndCoilAvailSched,    !- Availability Schedule Name",
		"    400.,                    !- U-Factor Times Area Value {W/K}",
		"    0.8,                     !- Maximum Water Flow Rate {m3/s}",
		"    Zone1VentSlabHWInletNode,!- Water Inlet Node Name",
		"    Zone1VentSlabHWOutletNode,  !- Water Outlet Node Name",
		"    Zone1VentSlabCCOutletNode,  !- Air Inlet Node Name",
		"    Zone1VentslabSlabInNode, !- Air Outlet Node Name",
		"    NominalCapacity,         !- Performance Input Method",
		"    40000,                   !- Rated Capacity {W}",
		"    ,                        !- Rated Inlet Water Temperature {C}",
		"    ,                        !- Rated Inlet Air Temperature {C}",
		"    ,                        !- Rated Outlet Water Temperature {C}",
		"    ,                        !- Rated Outlet Air Temperature {C}",
		"    ;                        !- Rated Ratio for Air and Water Convection",


		"  Coil:Heating:Electric,",
		"    Zone4VentSlabHeatingCoil,!- Name",
		"    FanAndCoilAvailSched,    !- Availability Schedule Name",
		"    0.99,                    !- Efficiency",
		"    40000,                   !- Nominal Capacity {W}",
		"    Zone4VentSlabCCOutletNode,  !- Air Inlet Node Name",
		"    Zone4VentSlabSlabInNode; !- Air Outlet Node Name",


		"  Branch,",
		"    Cooling Supply Inlet Branch,  !- Name",
		"    0,                       !- Maximum Flow Rate {m3/s}",
		"    ,                        !- Pressure Drop Curve Name",
		"    Pump:VariableSpeed,      !- Component 1 Object Type",
		"    ChW Circ Pump,           !- Component 1 Name",
		"    ChW Supply Inlet Node,   !- Component 1 Inlet Node Name",
		"    ChW Pump Outlet Node,    !- Component 1 Outlet Node Name",
		"    ACTIVE;                  !- Component 1 Branch Control Type",

		"  Branch,",
		"    Cooling Purchased Chilled Water Branch,  !- Name",
		"    0,                       !- Maximum Flow Rate {m3/s}",
		"    ,                        !- Pressure Drop Curve Name",
		"    DistrictCooling,         !- Component 1 Object Type",
		"    Purchased Cooling,       !- Component 1 Name",
		"    Purchased Cooling Inlet Node,  !- Component 1 Inlet Node Name",
		"    Purchased Cooling Outlet Node,  !- Component 1 Outlet Node Name",
		"    ACTIVE;                  !- Component 1 Branch Control Type",

		"  Branch,",
		"    Cooling Supply Bypass Branch,  !- Name",
		"    0,                       !- Maximum Flow Rate {m3/s}",
		"    ,                        !- Pressure Drop Curve Name",
		"    Pipe:Adiabatic,          !- Component 1 Object Type",
		"    Cooling Supply Side Bypass,  !- Component 1 Name",
		"    Cooling Supply Bypass Inlet Node,  !- Component 1 Inlet Node Name",
		"    Cooling Supply Bypass Outlet Node,  !- Component 1 Outlet Node Name",
		"    BYPASS;                  !- Component 1 Branch Control Type",

		"  Branch,",
		"    Cooling Supply Outlet Branch,  !- Name",
		"    0,                       !- Maximum Flow Rate {m3/s}",
		"    ,                        !- Pressure Drop Curve Name",
		"    Pipe:Adiabatic,          !- Component 1 Object Type",
		"    Cooling Supply Outlet,   !- Component 1 Name",
		"    Cooling Supply Exit Pipe Inlet Node,  !- Component 1 Inlet Node Name",
		"    ChW Supply Outlet Node,  !- Component 1 Outlet Node Name",
		"    PASSIVE;                 !- Component 1 Branch Control Type",

		"  Branch,",
		"    ZonesChWInletBranch,     !- Name",
		"    0,                       !- Maximum Flow Rate {m3/s}",
		"    ,                        !- Pressure Drop Curve Name",
		"    Pipe:Adiabatic,          !- Component 1 Object Type",
		"    ZonesChWInletPipe,       !- Component 1 Name",
		"    ChW Demand Inlet Node,   !- Component 1 Inlet Node Name",
		"    ChW Demand Entrance Pipe Outlet Node,  !- Component 1 Outlet Node Name",
		"    PASSIVE;                 !- Component 1 Branch Control Type",

		"  Branch,",
		"    ZonesChWOutletBranch,    !- Name",
		"    0,                       !- Maximum Flow Rate {m3/s}",
		"    ,                        !- Pressure Drop Curve Name",
		"    Pipe:Adiabatic,          !- Component 1 Object Type",
		"    ZonesChWOutletPipe,      !- Component 1 Name",
		"    ChW Demand Exit Pipe Inlet Node,  !- Component 1 Inlet Node Name",
		"    ChW Demand Outlet Node,  !- Component 1 Outlet Node Name",
		"    PASSIVE;                 !- Component 1 Branch Control Type",

		"  Branch,",
		"    Zone1ChWBranch,          !- Name",
		"    0,                       !- Maximum Flow Rate {m3/s}",
		"    ,                        !- Pressure Drop Curve Name",
		"    Coil:Cooling:Water,      !- Component 1 Object Type",
		"    Zone1VentSlabCoolingCoil,!- Component 1 Name",
		"    Zone1VentSlabChWInletNode,  !- Component 1 Inlet Node Name",
		"    Zone1VentSlabChWOutletNode,  !- Component 1 Outlet Node Name",
		"    ACTIVE;                  !- Component 1 Branch Control Type",

		"  Branch,",
		"    ZonesChWBypassBranch,    !- Name",
		"    0,                       !- Maximum Flow Rate {m3/s}",
		"    ,                        !- Pressure Drop Curve Name",
		"    Pipe:Adiabatic,          !- Component 1 Object Type",
		"    ZonesChWBypassPipe,      !- Component 1 Name",
		"    ZonesChWBypassInletNode, !- Component 1 Inlet Node Name",
		"    ZonesChWBypassOutletNode,!- Component 1 Outlet Node Name",
		"    BYPASS;                  !- Component 1 Branch Control Type",

		"  Branch,",
		"    Heating Supply Inlet Branch,  !- Name",
		"    0,                       !- Maximum Flow Rate {m3/s}",
		"    ,                        !- Pressure Drop Curve Name",
		"    Pump:VariableSpeed,      !- Component 1 Object Type",
		"    HW Circ Pump,            !- Component 1 Name",
		"    HW Supply Inlet Node,    !- Component 1 Inlet Node Name",
		"    HW Pump Outlet Node,     !- Component 1 Outlet Node Name",
		"    ACTIVE;                  !- Component 1 Branch Control Type",

		"  Branch,",
		"    Heating Purchased Hot Water Branch,  !- Name",
		"    0,                       !- Maximum Flow Rate {m3/s}",
		"    ,                        !- Pressure Drop Curve Name",
		"    DistrictHeating,         !- Component 1 Object Type",
		"    Purchased Heating,       !- Component 1 Name",
		"    Purchased Heat Inlet Node,  !- Component 1 Inlet Node Name",
		"    Purchased Heat Outlet Node,  !- Component 1 Outlet Node Name",
		"    ACTIVE;                  !- Component 1 Branch Control Type",

		"  Branch,",
		"    Heating Supply Bypass Branch,  !- Name",
		"    0,                       !- Maximum Flow Rate {m3/s}",
		"    ,                        !- Pressure Drop Curve Name",
		"    Pipe:Adiabatic,          !- Component 1 Object Type",
		"    Heating Supply Side Bypass,  !- Component 1 Name",
		"    Heating Supply Bypass Inlet Node,  !- Component 1 Inlet Node Name",
		"    Heating Supply Bypass Outlet Node,  !- Component 1 Outlet Node Name",
		"    BYPASS;                  !- Component 1 Branch Control Type",

		"  Branch,",
		"    Heating Supply Outlet Branch,  !- Name",
		"    0,                       !- Maximum Flow Rate {m3/s}",
		"    ,                        !- Pressure Drop Curve Name",
		"    Pipe:Adiabatic,          !- Component 1 Object Type",
		"    Heating Supply Outlet,   !- Component 1 Name",
		"    Heating Supply Exit Pipe Inlet Node,  !- Component 1 Inlet Node Name",
		"    HW Supply Outlet Node,   !- Component 1 Outlet Node Name",
		"    PASSIVE;                 !- Component 1 Branch Control Type",

		"  Branch,",
		"    ZonesHWInletBranch,      !- Name",
		"    0,                       !- Maximum Flow Rate {m3/s}",
		"    ,                        !- Pressure Drop Curve Name",
		"    Pipe:Adiabatic,          !- Component 1 Object Type",
		"    ZonesHWInletPipe,        !- Component 1 Name",
		"    HW Demand Inlet Node,    !- Component 1 Inlet Node Name",
		"    HW Demand Entrance Pipe Outlet Node,  !- Component 1 Outlet Node Name",
		"    PASSIVE;                 !- Component 1 Branch Control Type",

		"  Branch,",
		"    ZonesHWOutletBranch,     !- Name",
		"    0,                       !- Maximum Flow Rate {m3/s}",
		"    ,                        !- Pressure Drop Curve Name",
		"    Pipe:Adiabatic,          !- Component 1 Object Type",
		"    ZonesHWOutletPipe,       !- Component 1 Name",
		"    HW Demand Exit Pipe Inlet Node,  !- Component 1 Inlet Node Name",
		"    HW Demand Outlet Node,   !- Component 1 Outlet Node Name",
		"    PASSIVE;                 !- Component 1 Branch Control Type",

		"  Branch,",
		"    Zone1HWBranch,           !- Name",
		"    0,                       !- Maximum Flow Rate {m3/s}",
		"    ,                        !- Pressure Drop Curve Name",
		"    Coil:Heating:Water,      !- Component 1 Object Type",
		"    Zone1VentSlabHeatingCoil,!- Component 1 Name",
		"    Zone1VentSlabHWInletNode,!- Component 1 Inlet Node Name",
		"    Zone1VentSlabHWOutletNode,  !- Component 1 Outlet Node Name",
		"    ACTIVE;                  !- Component 1 Branch Control Type",

		"  Branch,",
		"    ZonesHWBypassBranch,     !- Name",
		"    0,                       !- Maximum Flow Rate {m3/s}",
		"    ,                        !- Pressure Drop Curve Name",
		"    Pipe:Adiabatic,          !- Component 1 Object Type",
		"    ZonesHWBypassPipe,       !- Component 1 Name",
		"    ZonesHWBypassInletNode,  !- Component 1 Inlet Node Name",
		"    ZonesHWBypassOutletNode, !- Component 1 Outlet Node Name",
		"    BYPASS;                  !- Component 1 Branch Control Type",

		"  Branch,",
		"    Zone4ChWBranch,          !- Name",
		"    0,                       !- Maximum Flow Rate {m3/s}",
		"    ,                        !- Pressure Drop Curve Name",
		"    Coil:Cooling:Water,      !- Component 1 Object Type",
		"    Zone4VentSlabCoolingCoil,!- Component 1 Name",
		"    Zone4VentSlabChWInletNode,  !- Component 1 Inlet Node Name",
		"    Zone4VentSlabChWOutletNode,  !- Component 1 Outlet Node Name",
		"    ACTIVE;                  !- Component 1 Branch Control Type",


		"  BranchList,",
		"    Cooling Supply Side Branches,  !- Name",
		"    Cooling Supply Inlet Branch,  !- Branch 1 Name",
		"    Cooling Purchased Chilled Water Branch,  !- Branch 2 Name",
		"    Cooling Supply Bypass Branch,  !- Branch 3 Name",
		"    Cooling Supply Outlet Branch;  !- Branch 4 Name",

		"  BranchList,",
		"    Cooling Demand Side Branches,  !- Name",
		"    ZonesChWInletBranch,     !- Branch 1 Name",
		"    Zone1ChWBranch,          !- Branch 2 Name",
		"    Zone4ChWBranch,          !- Branch 3 Name",
		"    ZonesChWBypassBranch,    !- Branch 4 Name",
		"    ZonesChWOutletBranch;    !- Branch 5 Name",

		"  BranchList,",
		"    Heating Supply Side Branches,  !- Name",
		"    Heating Supply Inlet Branch,  !- Branch 1 Name",
		"    Heating Purchased Hot Water Branch,  !- Branch 2 Name",
		"    Heating Supply Bypass Branch,  !- Branch 3 Name",
		"    Heating Supply Outlet Branch;  !- Branch 4 Name",

		"  BranchList,",
		"    Heating Demand Side Branches,  !- Name",
		"    ZonesHWInletBranch,      !- Branch 1 Name",
		"    Zone1HWBranch,           !- Branch 2 Name",
		"    ZonesHWBypassBranch,     !- Branch 3 Name",
		"    ZonesHWOutletBranch;     !- Branch 4 Name",


		"  Connector:Splitter,",
		"    Cooling Supply Splitter, !- Name",
		"    Cooling Supply Inlet Branch,  !- Inlet Branch Name",
		"    Cooling Purchased Chilled Water Branch,  !- Outlet Branch 1 Name",
		"    Cooling Supply Bypass Branch;  !- Outlet Branch 2 Name",

		"  Connector:Splitter,",
		"    Zones ChW Splitter,      !- Name",
		"    ZonesChWInletBranch,     !- Inlet Branch Name",
		"    Zone1ChWBranch,          !- Outlet Branch 1 Name",
		"    Zone4ChWBranch,          !- Outlet Branch 2 Name",
		"    ZonesChWBypassBranch;    !- Outlet Branch 3 Name",

		"  Connector:Splitter,",
		"    Heating Supply Splitter, !- Name",
		"    Heating Supply Inlet Branch,  !- Inlet Branch Name",
		"    Heating Purchased Hot Water Branch,  !- Outlet Branch 1 Name",
		"    Heating Supply Bypass Branch;  !- Outlet Branch 2 Name",

		"  Connector:Splitter,",
		"    Zones HW Splitter,       !- Name",
		"    ZonesHWInletBranch,      !- Inlet Branch Name",
		"    Zone1HWBranch,           !- Outlet Branch 1 Name",
		"    ZonesHWBypassBranch;     !- Outlet Branch 2 Name",


		"  Connector:Mixer,",
		"    Cooling Supply Mixer,    !- Name",
		"    Cooling Supply Outlet Branch,  !- Outlet Branch Name",
		"    Cooling Purchased Chilled Water Branch,  !- Inlet Branch 1 Name",
		"    Cooling Supply Bypass Branch;  !- Inlet Branch 2 Name",

		"  Connector:Mixer,",
		"    Zones ChW Mixer,         !- Name",
		"    ZonesChWOutletBranch,    !- Outlet Branch Name",
		"    Zone1ChWBranch,          !- Inlet Branch 1 Name",
		"    Zone4ChWBranch,          !- Inlet Branch 2 Name",
		"    ZonesChWBypassBranch;    !- Inlet Branch 3 Name",

		"  Connector:Mixer,",
		"    Heating Supply Mixer,    !- Name",
		"    Heating Supply Outlet Branch,  !- Outlet Branch Name",
		"    Heating Purchased Hot Water Branch,  !- Inlet Branch 1 Name",
		"    Heating Supply Bypass Branch;  !- Inlet Branch 2 Name",

		"  Connector:Mixer,",
		"    Zones HW Mixer,          !- Name",
		"    ZonesHWOutletBranch,     !- Outlet Branch Name",
		"    Zone1HWBranch,           !- Inlet Branch 1 Name",
		"    ZonesHWBypassBranch;     !- Inlet Branch 2 Name",


		"  ConnectorList,",
		"    Cooling Supply Side Connectors,  !- Name",
		"    Connector:Splitter,      !- Connector 1 Object Type",
		"    Cooling Supply Splitter, !- Connector 1 Name",
		"    Connector:Mixer,         !- Connector 2 Object Type",
		"    Cooling Supply Mixer;    !- Connector 2 Name",

		"  ConnectorList,",
		"    Cooling Demand Side Connectors,  !- Name",
		"    Connector:Splitter,      !- Connector 1 Object Type",
		"    Zones ChW Splitter,      !- Connector 1 Name",
		"    Connector:Mixer,         !- Connector 2 Object Type",
		"    Zones ChW Mixer;         !- Connector 2 Name",

		"  ConnectorList,",
		"    Heating Supply Side Connectors,  !- Name",
		"    Connector:Splitter,      !- Connector 1 Object Type",
		"    Heating Supply Splitter, !- Connector 1 Name",
		"    Connector:Mixer,         !- Connector 2 Object Type",
		"    Heating Supply Mixer;    !- Connector 2 Name",

		"  ConnectorList,",
		"    Heating Demand Side Connectors,  !- Name",
		"    Connector:Splitter,      !- Connector 1 Object Type",
		"    Zones HW Splitter,       !- Connector 1 Name",
		"    Connector:Mixer,         !- Connector 2 Object Type",
		"    Zones HW Mixer;          !- Connector 2 Name",


		"  NodeList,",
		"    Chilled Water Loop Setpoint Node List,  !- Name",
		"    ChW Supply Outlet Node;  !- Node 1 Name",

		"  NodeList,",
		"    Hot Water Loop Setpoint Node List,  !- Name",
		"    HW Supply Outlet Node;   !- Node 1 Name",


		"  OutdoorAir:Node,",
		"    Zone1VentSlabOAInNode,   !- Name",
		"    -1.0;                    !- Height Above Ground {m}",

		"  OutdoorAir:Node,",
		"    Zone4VentSlabOAInNode,   !- Name",
		"    -1.0;                    !- Height Above Ground {m}",


		"  Pump:VariableSpeed,",
		"    ChW Circ Pump,           !- Name",
		"    ChW Supply Inlet Node,   !- Inlet Node Name",
		"    ChW Pump Outlet Node,    !- Outlet Node Name",
		"    0.004,                   !- Rated Flow Rate {m3/s}",
		"    300000,                  !- Rated Pump Head {Pa}",
		"    1800,                    !- Rated Power Consumption {W}",
		"    0.87,                    !- Motor Efficiency",
		"    0.0,                     !- Fraction of Motor Inefficiencies to Fluid Stream",
		"    0,                       !- Coefficient 1 of the Part Load Performance Curve",
		"    1,                       !- Coefficient 2 of the Part Load Performance Curve",
		"    0,                       !- Coefficient 3 of the Part Load Performance Curve",
		"    0,                       !- Coefficient 4 of the Part Load Performance Curve",
		"    0,                       !- Minimum Flow Rate {m3/s}",
		"    INTERMITTENT;            !- Pump Control Type",

		"  Pump:VariableSpeed,",
		"    HW Circ Pump,            !- Name",
		"    HW Supply Inlet Node,    !- Inlet Node Name",
		"    HW Pump Outlet Node,     !- Outlet Node Name",
		"    0.002,                   !- Rated Flow Rate {m3/s}",
		"    300000,                  !- Rated Pump Head {Pa}",
		"    900,                     !- Rated Power Consumption {W}",
		"    0.87,                    !- Motor Efficiency",
		"    0.0,                     !- Fraction of Motor Inefficiencies to Fluid Stream",
		"    0,                       !- Coefficient 1 of the Part Load Performance Curve",
		"    1,                       !- Coefficient 2 of the Part Load Performance Curve",
		"    0,                       !- Coefficient 3 of the Part Load Performance Curve",
		"    0,                       !- Coefficient 4 of the Part Load Performance Curve",
		"    0,                       !- Minimum Flow Rate {m3/s}",
		"    INTERMITTENT;            !- Pump Control Type",


		"  DistrictCooling,",
		"    Purchased Cooling,       !- Name",
		"    Purchased Cooling Inlet Node,  !- Chilled Water Inlet Node Name",
		"    Purchased Cooling Outlet Node,  !- Chilled Water Outlet Node Name",
		"    1000000;                 !- Nominal Capacity {W}",


		"  DistrictHeating,",
		"    Purchased Heating,       !- Name",
		"    Purchased Heat Inlet Node,  !- Hot Water Inlet Node Name",
		"    Purchased Heat Outlet Node,  !- Hot Water Outlet Node Name",
		"    1000000;                 !- Nominal Capacity {W}",


		"  PlantLoop,",
		"    Chilled Water Loop,      !- Name",
		"    Water,                   !- Fluid Type",
		"    ,                        !- User Defined Fluid Type",
		"    Chilled Loop Operation,  !- Plant Equipment Operation Scheme Name",
		"    ChW Supply Outlet Node,  !- Loop Temperature Setpoint Node Name",
		"    100,                     !- Maximum Loop Temperature {C}",
		"    1,                       !- Minimum Loop Temperature {C}",
		"    0.004,                   !- Maximum Loop Flow Rate {m3/s}",
		"    0.0,                     !- Minimum Loop Flow Rate {m3/s}",
		"    autocalculate,           !- Plant Loop Volume {m3}",
		"    ChW Supply Inlet Node,   !- Plant Side Inlet Node Name",
		"    ChW Supply Outlet Node,  !- Plant Side Outlet Node Name",
		"    Cooling Supply Side Branches,  !- Plant Side Branch List Name",
		"    Cooling Supply Side Connectors,  !- Plant Side Connector List Name",
		"    ChW Demand Inlet Node,   !- Demand Side Inlet Node Name",
		"    ChW Demand Outlet Node,  !- Demand Side Outlet Node Name",
		"    Cooling Demand Side Branches,  !- Demand Side Branch List Name",
		"    Cooling Demand Side Connectors,  !- Demand Side Connector List Name",
		"    Optimal;                 !- Load Distribution Scheme",

		"  PlantLoop,",
		"    Hot Water Loop,          !- Name",
		"    Water,                   !- Fluid Type",
		"    ,                        !- User Defined Fluid Type",
		"    Hot Loop Operation,      !- Plant Equipment Operation Scheme Name",
		"    HW Supply Outlet Node,   !- Loop Temperature Setpoint Node Name",
		"    100,                     !- Maximum Loop Temperature {C}",
		"    10,                      !- Minimum Loop Temperature {C}",
		"    0.002,                   !- Maximum Loop Flow Rate {m3/s}",
		"    0.0,                     !- Minimum Loop Flow Rate {m3/s}",
		"    autocalculate,           !- Plant Loop Volume {m3}",
		"    HW Supply Inlet Node,    !- Plant Side Inlet Node Name",
		"    HW Supply Outlet Node,   !- Plant Side Outlet Node Name",
		"    Heating Supply Side Branches,  !- Plant Side Branch List Name",
		"    Heating Supply Side Connectors,  !- Plant Side Connector List Name",
		"    HW Demand Inlet Node,    !- Demand Side Inlet Node Name",
		"    HW Demand Outlet Node,   !- Demand Side Outlet Node Name",
		"    Heating Demand Side Branches,  !- Demand Side Branch List Name",
		"    Heating Demand Side Connectors,  !- Demand Side Connector List Name",
		"    Optimal;                 !- Load Distribution Scheme",


		"  Pipe:Adiabatic,",
		"    Cooling Supply Side Bypass,  !- Name",
		"    Cooling Supply Bypass Inlet Node,  !- Inlet Node Name",
		"    Cooling Supply Bypass Outlet Node;  !- Outlet Node Name",

		"  Pipe:Adiabatic,",
		"    Cooling Supply Outlet,   !- Name",
		"    Cooling Supply Exit Pipe Inlet Node,  !- Inlet Node Name",
		"    ChW Supply Outlet Node;  !- Outlet Node Name",

		"  Pipe:Adiabatic,",
		"    ZonesChWInletPipe,       !- Name",
		"    ChW Demand Inlet Node,   !- Inlet Node Name",
		"    ChW Demand Entrance Pipe Outlet Node;  !- Outlet Node Name",

		"  Pipe:Adiabatic,",
		"    ZonesChWOutletPipe,      !- Name",
		"    ChW Demand Exit Pipe Inlet Node,  !- Inlet Node Name",
		"    ChW Demand Outlet Node;  !- Outlet Node Name",

		"  Pipe:Adiabatic,",
		"    ZonesChWBypassPipe,      !- Name",
		"    ZonesChWBypassInletNode, !- Inlet Node Name",
		"    ZonesChWBypassOutletNode;!- Outlet Node Name",

		"  Pipe:Adiabatic,",
		"    Heating Supply Side Bypass,  !- Name",
		"    Heating Supply Bypass Inlet Node,  !- Inlet Node Name",
		"    Heating Supply Bypass Outlet Node;  !- Outlet Node Name",

		"  Pipe:Adiabatic,",
		"    Heating Supply Outlet,   !- Name",
		"    Heating Supply Exit Pipe Inlet Node,  !- Inlet Node Name",
		"    HW Supply Outlet Node;   !- Outlet Node Name",

		"  Pipe:Adiabatic,",
		"    ZonesHWInletPipe,        !- Name",
		"    HW Demand Inlet Node,    !- Inlet Node Name",
		"    HW Demand Entrance Pipe Outlet Node;  !- Outlet Node Name",

		"  Pipe:Adiabatic,",
		"    ZonesHWOutletPipe,       !- Name",
		"    HW Demand Exit Pipe Inlet Node,  !- Inlet Node Name",
		"    HW Demand Outlet Node;   !- Outlet Node Name",

		"  Pipe:Adiabatic,",
		"    ZonesHWBypassPipe,       !- Name",
		"    ZonesHWBypassInletNode,  !- Inlet Node Name",
		"    ZonesHWBypassOutletNode; !- Outlet Node Name",


		"  PlantEquipmentList,",
		"    cooling plant,           !- Name",
		"    DistrictCooling,         !- Equipment 1 Object Type",
		"    Purchased Cooling;       !- Equipment 1 Name",

		"  PlantEquipmentList,",
		"    heating plant,           !- Name",
		"    DistrictHeating,         !- Equipment 1 Object Type",
		"    Purchased Heating;       !- Equipment 1 Name",

		"  PlantEquipmentOperation:CoolingLoad,",
		"    Purchased Cooling Only,  !- Name",
		"    0,                       !- Load Range 1 Lower Limit {W}",
		"    1000000,                 !- Load Range 1 Upper Limit {W}",
		"    cooling plant;           !- Range 1 Equipment List Name",


		"  PlantEquipmentOperation:HeatingLoad,",
		"    Purchased Heating Only,  !- Name",
		"    0,                       !- Load Range 1 Lower Limit {W}",
		"    1000000,                 !- Load Range 1 Upper Limit {W}",
		"    heating plant;           !- Range 1 Equipment List Name",


		"  PlantEquipmentOperationSchemes,",
		"    Chilled Loop Operation,  !- Name",
		"    PlantEquipmentOperation:CoolingLoad,  !- Control Scheme 1 Object Type",
		"    Purchased Cooling Only,  !- Control Scheme 1 Name",
		"    ON;                      !- Control Scheme 1 Schedule Name",

		"  PlantEquipmentOperationSchemes,",
		"    Hot Loop Operation,      !- Name",
		"    PlantEquipmentOperation:HeatingLoad,  !- Control Scheme 1 Object Type",
		"    Purchased Heating Only,  !- Control Scheme 1 Name",
		"    ON;                      !- Control Scheme 1 Schedule Name",


		"  SetpointManager:Scheduled,",
		"    Chilled Water Loop Setpoint Manager,  !- Name",
		"    Temperature,             !- Control Variable",
		"    CW Loop Temp Schedule,   !- Schedule Name",
		"    Chilled Water Loop Setpoint Node List;  !- Setpoint Node or NodeList Name",

		"  SetpointManager:Scheduled,",
		"    Hot Water Loop Setpoint Manager,  !- Name",
		"    Temperature,             !- Control Variable",
		"    HW Loop Temp Schedule,   !- Schedule Name",
		"    Hot Water Loop Setpoint Node List;  !- Setpoint Node or NodeList Name",
		} );
		ASSERT_FALSE( process_idf( idf_objects ) );

		NumOfTimeStepInHour = 1; // must initialize this to get schedules initialized
		MinutesPerTimeStep = 60; // must initialize this to get schedules initialized
		ProcessScheduleInput();

		HeatBalanceManager::GetZoneData( ErrorsFound ); // read zone data
		EXPECT_FALSE( ErrorsFound );

		ErrorsFound = false;
		GetMaterialData( ErrorsFound );
		EXPECT_FALSE( ErrorsFound );

		ErrorsFound = false;
		HeatBalanceManager::GetConstructData( ErrorsFound ); // If errors found in input
		EXPECT_FALSE( ErrorsFound );

		ErrorsFound = false;
		SetupZoneGeometry( ErrorsFound ); // If errors found in input
		EXPECT_FALSE( ErrorsFound );

		ErrorsFound = false;
		GetSurfaceData( ErrorsFound ); // If errors found in input
		EXPECT_FALSE( ErrorsFound );

		ErrorsFound = false;
		GetSurfaceListsInputs();
		EXPECT_FALSE( ErrorsFound );
		

		GetVentilatedSlabInput();

		InitVentilatedSlab( Item, VentSlabZoneNum, FirstHVACIteration );

	// Deallocate everything
	VentSlab.deallocate();
	Node.deallocate();

}
