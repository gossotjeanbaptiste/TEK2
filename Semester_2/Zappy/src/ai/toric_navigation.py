class ToricNavigator:
    NORTH = 0
    EAST = 1
    SOUTH = 2
    WEST = 3

    def __init__(self, width=0, height=0):
        self.width = width
        self.height = height
        self.x = 0
        self.y = 0
        self.direction = self.NORTH

    def set_world_size(self, width, height):
        self.width = max(0, int(width))
        self.height = max(0, int(height))

    def set_position(self, x, y, direction=None):
        self.x = int(x)
        self.y = int(y)
        if direction is not None:
            self.direction = direction % 4

    def turn_left(self):
        self.direction = (self.direction - 1) % 4

    def turn_right(self):
        self.direction = (self.direction + 1) % 4

    def step_forward(self):
        if self.direction == self.NORTH:
            self.y -= 1
        elif self.direction == self.EAST:
            self.x += 1
        elif self.direction == self.SOUTH:
            self.y += 1
        else:
            self.x -= 1
        self.x, self.y = self._wrap(self.x, self.y)

    def _wrap(self, x, y):
        if self.width > 0:
            x %= self.width
        if self.height > 0:
            y %= self.height
        return x, y

    def _vision_row_bounds(self, index):
        if index <= 0:
            return 0, 0
        row = 1
        while (row + 1) * (row + 1) <= index:
            row += 1
        first_index = row * row
        center_index = first_index + row
        return first_index, center_index

    def action_from_vision_index(self, index):
        _, center_index = self._vision_row_bounds(index)
        if index == center_index:
            return "Forward"
        if index < center_index:
            return "Left"
        return "Right"

    def best_visible_action(self, look_result, wanted_resources):
        if not isinstance(look_result, list):
            return None
        if isinstance(wanted_resources, str):
            wanted_resources = [wanted_resources]
        for resource_name in wanted_resources:
            for index, tile in enumerate(look_result):
                if index == 0:
                    continue
                if resource_name not in str(tile).split():
                    continue
                return self.action_from_vision_index(index)
        return None

    def best_visible_target(self, look_result, resource_name="food"):
        if not isinstance(look_result, list):
            return None
        for index, tile in enumerate(look_result):
            if index == 0:
                continue
            if resource_name in str(tile).split():
                return index
        return None
