extends Dungen

@onready var tile_map := $TileMap
# Called when the node enters the scene tree for the first time.
func _ready() -> void:
  generate()

  var map = get_all()
  
  for p in map.paths:
    draw_tilemap_rectangle(p)
  for r in map.rooms:
    draw_tilemap_rectangle(r)     


func draw_tilemap_rectangle(rect: Rect2i) -> void:
  for x in range(rect.position.x, rect.position.x + rect.size.x):
    for y in range(rect.position.y, rect.position.y + rect.size.y):
      tile_map.set_cell(0, Vector2i(x, y), 0, Vector2i(0, 0))


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta: float) -> void:
  pass
