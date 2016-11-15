enum {
	COMPONENT_NONE = 0,
	COMPONENT_POSITION = 1 << 0,
	COMPONENT_VELOCITY = 1 << 1,
	COMPONENT_APPEARANCE = 1 << 2,
	COMPONENT_MASS = 1 << 3,
	COMPONENT_COLLISION = 1 << 4,
	COMPONENT_PLAYER = 1 << 5
	//MORE
} Component;

//Lista di tutti i componenti utilizzabili
struct PositionComponent {
	float x, y;
};

struct VelocityComponent {
	float speedX, speedY;
};

struct AppearanceComponent {
	//TO-DO
};

struct MassComponent {
	float mass;
	//TO-DO
};

struct CollisionComponent {
	//TO-DO
};

struct PlayerComponent {
	char* name;
	float health; //possibile che necessiti un componente Health
	//TO-DO
};