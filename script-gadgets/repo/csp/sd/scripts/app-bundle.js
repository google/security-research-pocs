define('environment',["exports"], function (exports) {
  "use strict";

  Object.defineProperty(exports, "__esModule", {
    value: true
  });
  exports.default = {
    debug: true,
    testing: true
  };
});
define('main',['exports'], function (exports) {
  'use strict';

  Object.defineProperty(exports, "__esModule", {
    value: true
  });
  exports.configure = configure;
  function configure(aurelia) {
    aurelia.use.basicConfiguration().globalResources('todos');
    aurelia.start().then(function () {
      return aurelia.enhance(document.querySelector('todos'));
    });
  }
});
define('todo',["exports"], function (exports) {
  "use strict";

  Object.defineProperty(exports, "__esModule", {
    value: true
  });

  function _classCallCheck(instance, Constructor) {
    if (!(instance instanceof Constructor)) {
      throw new TypeError("Cannot call a class as a function");
    }
  }

  var Todo = exports.Todo = function Todo(description) {
    _classCallCheck(this, Todo);

    this.description = description;
    this.done = false;
  };
});
define('todos',['exports', './todo'], function (exports, _todo) {
  'use strict';

  Object.defineProperty(exports, "__esModule", {
    value: true
  });
  exports.Todos = undefined;

  function _classCallCheck(instance, Constructor) {
    if (!(instance instanceof Constructor)) {
      throw new TypeError("Cannot call a class as a function");
    }
  }

  var Todos = exports.Todos = function () {
    function Todos() {
      _classCallCheck(this, Todos);

      this.heading = "Todos";
      this.todos = [];
      this.todoDescription = '';
    }

    Todos.prototype.addTodo = function addTodo() {
      if (this.todoDescription) {
        this.todos.push(new _todo.Todo(this.todoDescription));
        this.todoDescription = '';
      }
    };

    Todos.prototype.removeTodo = function removeTodo(todo) {
      var index = this.todos.indexOf(todo);
      if (index !== -1) {
        this.todos.splice(index, 1);
      }
    };

    return Todos;
  }();
});
define('resources/index',["exports"], function (exports) {
  "use strict";

  Object.defineProperty(exports, "__esModule", {
    value: true
  });
  exports.configure = configure;
  function configure(config) {}
});
define('text!todos.html', ['module'], function(module) { module.exports = "<template>\n  <h1>${heading}</h1>\n\n  <form submit.trigger=\"addTodo()\">\n    <input type=\"text\" value.bind=\"todoDescription\">\n    <button type=\"submit\">Add Todo</button>\n  </form>\n\n  <ul>\n    <li repeat.for=\"todo of todos\">\n      <input type=\"checkbox\" checked.bind=\"todo.done\">\n      <span css=\"text-decoration: ${todo.done ? 'line-through' : 'none'}\">\n        ${todo.description}\n      </span>\n      <button click.trigger=\"removeTodo(todo)\">Remove</button>\n    </li>    \n  </ul>\n</template>"; });
//# sourceMappingURL=app-bundle.js.map