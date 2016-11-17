enum Component {
	COMPONENT_NONE = 0,
	COMPONENT_POSITION = 1 << 0,
	COMPONENT_VELOCITY = 1 << 1,
	COMPONENT_APPEARANCE = 1 << 2,
	COMPONENT_BODY = 1 << 3,
	COMPONENT_COLLISION = 1 << 4,
	COMPONENT_PLAYER = 1 << 5
	//MORE
};

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

struct BodyComponent {
	float mass, elasticity;
	//TO-DO
};

struct CollisionComponent {
	//TO-DO
};

struct PlayerComponent {
	char* name;
	//TO-DO
};