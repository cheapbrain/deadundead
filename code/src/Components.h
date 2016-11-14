typedef enum {
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
typedef struct {
	float x, y;
} PositionComponent;

typedef struct {
	float speedX, speedY;
} VelocityComponent;

typedef struct {
	//TO-DO
} AppearanceComponent;

typedef struct {
	float mass;
	//TO-DO
} MassComponent;

typedef struct {
	//TO-DO
} CollisionComponent;

typedef struct {
	char* name;
	float health; //possibile che necessiti un componente Health
	//TO-DO
} PlayerComponent