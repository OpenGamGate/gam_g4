#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"

// #include <getopt.h>
#include <cstdlib>
#include <queue>
#include <locale.h>

#include "G4UImanager.hh"
#include "G4UIterminal.hh"
#include "G4UItcsh.hh"

#include "G4VUserDetectorConstruction.hh"
#include "G4NistManager.hh"
#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4RunManager.hh"
#include "G4VModularPhysicsList.hh"
#include "QBBC.hh"
#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4ParticleGun.hh"

#include "G4UserRunAction.hh"
#include "G4UserSteppingAction.hh"
#include "G4UserEventAction.hh"
#include "G4VUserActionInitialization.hh"


class Test1DetectorConstruction : public G4VUserDetectorConstruction {
public:
    Test1DetectorConstruction() :
        G4VUserDetectorConstruction() {}

    virtual ~Test1DetectorConstruction() {}

    virtual G4VPhysicalVolume *Construct() {
        // Get nist material manager
        G4NistManager *nist = G4NistManager::Instance();

        G4bool checkOverlaps = true;

        // world
        G4Material *world_mat = nist->FindOrBuildMaterial("G4_AIR");
        G4Box *solidWorld =
            new G4Box("World", 5 * m, 5 * m, 5 * m);
        G4LogicalVolume *logicWorld =
            new G4LogicalVolume(solidWorld,          //its solid
                                world_mat,           //its material
                                "World");            //its name
        G4VPhysicalVolume *physWorld =
            new G4PVPlacement(0,                     //no rotation
                              G4ThreeVector(),       //at (0,0,0)
                              logicWorld,            //its logical volume
                              "World",               //its name
                              0,                     //its mother  volume
                              false,                 //no boolean operation
                              0,                     //copy number
                              checkOverlaps);        //overlaps checking

        // waterbox
        G4Material *env_mat = nist->FindOrBuildMaterial("G4_WATER");

        G4Box *solidEnv =
            new G4Box("waterbox", 20 * cm, 20 * cm, 20 * cm);
        G4LogicalVolume *logicEnv =
            new G4LogicalVolume(solidEnv,            //its solid
                                env_mat,             //its material
                                "waterbox");         //its name
        auto position = G4ThreeVector(0, 0, 25 * cm);
        new G4PVPlacement(0,                       //no rotation
                          position,                //at (0,0,25)
                          logicEnv,                //its logical volume
                          "waterbox",              //its name
                          logicWorld,              //its mother  volume
                          false,                   //no boolean operation
                          0,                       //copy number
                          checkOverlaps);          //overlaps checking
        return physWorld;
    }

};

class Test1PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction {
public:
    Test1PrimaryGeneratorAction() {
        G4int n_particle = 1;
        fParticleGun = new G4ParticleGun(n_particle);

        // default particle kinematic
        G4ParticleTable *particleTable = G4ParticleTable::GetParticleTable();
        G4String particleName;
        G4ParticleDefinition *particle
            = particleTable->FindParticle(particleName = "proton");
        fParticleGun->SetParticleDefinition(particle);
        fParticleGun->SetParticleMomentumDirection(G4ThreeVector(0., 0., 1.));
        fParticleGun->SetParticleEnergy(150. * MeV);
    }

    virtual ~Test1PrimaryGeneratorAction() {}

    // method from the base class
    virtual void GeneratePrimaries(G4Event *anEvent) {
        G4double size = 3 * cm;
        G4double x0 = size * (G4UniformRand() - 0.5);
        G4double y0 = size * (G4UniformRand() - 0.5);
        G4double z0 = 0;
        std::cout << "position particle " << x0 << " " << y0 << " " << z0 << std::endl;
        fParticleGun->SetParticlePosition(G4ThreeVector(x0, y0, z0));
        fParticleGun->GeneratePrimaryVertex(anEvent);
    }

    // method to access particle gun
    const G4ParticleGun *GetParticleGun() const { return fParticleGun; }

private:
    G4ParticleGun *fParticleGun; // pointer a to G4 gun class
};

//-----------------------------------------------------------------------------
class B1RunAction : public G4UserRunAction {
public:
    B1RunAction() : G4UserRunAction() {}

    virtual ~B1RunAction() {}

    virtual void BeginOfRunAction(const G4Run *) {
        std::cout << "Start RUN " << std::endl;
    }

    virtual void EndOfRunAction(const G4Run *) {}
};

//-----------------------------------------------------------------------------
class B1EventAction : public G4UserEventAction {
public:
    B1EventAction(B1RunAction * /*runAction*/) : G4UserEventAction() {}

    virtual ~B1EventAction() {}

    virtual void BeginOfEventAction(const G4Event *event) {
        std::cout << "BeginOfEventAction " << event->GetEventID() << std::endl;
    }

    virtual void EndOfEventAction(const G4Event * /*event*/) {}
};

//-----------------------------------------------------------------------------
class B1SteppingAction : public G4UserSteppingAction {
public:
    B1SteppingAction(B1EventAction * /*eventAction*/) : G4UserSteppingAction() {}

    virtual ~B1SteppingAction() {}

    // method from the base class
    virtual void UserSteppingAction(const G4Step *step) {
        std::cout << "UserSteppingAction " << step->GetTotalEnergyDeposit() << std::endl;
    }
};

//-----------------------------------------------------------------------------
class B1ActionInitialization : public G4VUserActionInitialization {
public:
    B1ActionInitialization() : G4VUserActionInitialization() {}

    virtual ~B1ActionInitialization() {}

    virtual void BuildForMaster() const {
        B1RunAction *runAction = new B1RunAction;
        SetUserAction(runAction);
    }

    virtual void Build() const {
        SetUserAction(new Test1PrimaryGeneratorAction);
        B1RunAction *runAction = new B1RunAction;
        SetUserAction(runAction);
        B1EventAction *eventAction = new B1EventAction(runAction);
        SetUserAction(eventAction);
        SetUserAction(new B1SteppingAction(eventAction));
    }
};


//-----------------------------------------------------------------------------
int main() {

    std::cout << "hello world" << std::endl;
    // construct the default run manager
    G4RunManager *runManager = new G4RunManager;

    // Physics list
    G4VModularPhysicsList *physicsList = new QBBC;
    physicsList->SetVerboseLevel(1);
    runManager->SetUserInitialization(physicsList);

    // set mandatory initialization classes
    runManager->SetUserInitialization(new Test1DetectorConstruction);
    runManager->SetUserInitialization(physicsList);
    //runManager->SetUserAction(new Test1PrimaryGeneratorAction);
    runManager->SetUserInitialization(new B1ActionInitialization);

    // initialize G4 kernel
    runManager->Initialize();

    // get the pointer to the UI manager and set verbosities
    G4UImanager *UI = G4UImanager::GetUIpointer();
    UI->ApplyCommand("/run/verbose 2");
    UI->ApplyCommand("/event/verbose 2");
    UI->ApplyCommand("/tracking/verbose 2");

    // start a run
    int numberOfEvent = 10;
    runManager->BeamOn(numberOfEvent);

    // job termination
    delete runManager;
    return 0;
}
//-----------------------------------------------------------------------------
