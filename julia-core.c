/*
  Copyright (C) 2016 by Syohei YOSHIDA

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <emacs-module.h>

#include <julia.h>

int plugin_is_GPL_compatible;

static emacs_value
Fjulia_eval(emacs_env *env, ptrdiff_t nargs, emacs_value args[], void *data)
{
	emacs_value code = args[0];
	ptrdiff_t size = 0;

	env->copy_string_contents(env, code, NULL, &size);
	char *code_buf = malloc(size);
	if (code_buf == NULL)
		return env->intern(env, "nil");
	env->copy_string_contents(env, code, code_buf, &size);

	jl_value_t *ret;
	ret = jl_eval_string(code_buf);
	if (jl_exception_occurred()) {
		jl_show(jl_stderr_obj(), jl_exception_occurred());
		jl_printf(jl_stderr_stream(), "\n");
		return env->intern(env, "nil");
	}

	if (!jl_is_string(ret))
		return env->intern(env, "nil");

	return env->make_string(env, jl_string_data(ret), jl_string_len(ret));
}

static void
bind_function(emacs_env *env, const char *name, emacs_value Sfun)
{
	emacs_value Qfset = env->intern(env, "fset");
	emacs_value Qsym = env->intern(env, name);
	emacs_value args[] = { Qsym, Sfun };

	env->funcall(env, Qfset, 2, args);
}

static void
provide(emacs_env *env, const char *feature)
{
	emacs_value Qfeat = env->intern(env, feature);
	emacs_value Qprovide = env->intern (env, "provide");
	emacs_value args[] = { Qfeat };

	env->funcall(env, Qprovide, 1, args);
}

int
emacs_module_init(struct emacs_runtime *ert)
{
	emacs_env *env = ert->get_environment(ert);

#define DEFUN(lsym, csym, amin, amax, doc, data) \
	bind_function (env, lsym, env->make_function(env, amin, amax, csym, doc, data))

	DEFUN("julia-core-eval", Fjulia_eval, 1, 1, NULL, NULL);

#undef DEFUN

	provide(env, "julia-core");

	// Julia initialization
	//jl_init(JULIA_INIT_DIR);
	return 0;
}

/*
  Local Variables:
  c-basic-offset: 8
  indent-tabs-mode: t
  End:
*/
