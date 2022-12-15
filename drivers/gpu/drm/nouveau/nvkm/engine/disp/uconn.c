/*
 * Copyright 2021 Red Hat Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE COPYRIGHT HOLDER(S) OR AUTHOR(S) BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */
#define nvkm_uconn(p) container_of((p), struct nvkm_conn, object)
#include "conn.h"

#include <subdev/gpio.h>

#include <nvif/if0011.h>

static int
nvkm_uconn_mthd_hpd_status(struct nvkm_conn *conn, void *argv, u32 argc)
{
	struct nvkm_gpio *gpio = conn->disp->engine.subdev.device->gpio;
	union nvif_conn_hpd_status_args *args = argv;

	if (argc != sizeof(args->v0) || args->v0.version != 0)
		return -ENOSYS;

	args->v0.support = gpio && conn->info.hpd != DCB_GPIO_UNUSED;
	args->v0.present = 0;

	if (args->v0.support) {
		int ret = nvkm_gpio_get(gpio, 0, DCB_GPIO_UNUSED, conn->info.hpd);

		if (WARN_ON(ret < 0)) {
			args->v0.support = false;
			return 0;
		}

		args->v0.present = ret;
	}

	return 0;
}

static int
nvkm_uconn_mthd(struct nvkm_object *object, u32 mthd, void *argv, u32 argc)
{
	struct nvkm_conn *conn = nvkm_uconn(object);

	switch (mthd) {
	case NVIF_CONN_V0_HPD_STATUS: return nvkm_uconn_mthd_hpd_status(conn, argv, argc);
	default:
		break;
	}

	return -EINVAL;
}

static void *
nvkm_uconn_dtor(struct nvkm_object *object)
{
	struct nvkm_conn *conn = nvkm_uconn(object);
	struct nvkm_disp *disp = conn->disp;

	spin_lock(&disp->client.lock);
	conn->object.func = NULL;
	spin_unlock(&disp->client.lock);
	return NULL;
}

static const struct nvkm_object_func
nvkm_uconn = {
	.dtor = nvkm_uconn_dtor,
	.mthd = nvkm_uconn_mthd,
};

int
nvkm_uconn_new(const struct nvkm_oclass *oclass, void *argv, u32 argc, struct nvkm_object **pobject)
{
	struct nvkm_disp *disp = nvkm_udisp(oclass->parent);
	struct nvkm_conn *cont, *conn = NULL;
	union nvif_conn_args *args = argv;
	int ret;

	if (argc != sizeof(args->v0) || args->v0.version != 0)
		return -ENOSYS;

	list_for_each_entry(cont, &disp->conns, head) {
		if (cont->index == args->v0.id) {
			conn = cont;
			break;
		}
	}

	if (!conn)
		return -EINVAL;

	ret = -EBUSY;
	spin_lock(&disp->client.lock);
	if (!conn->object.func) {
		nvkm_object_ctor(&nvkm_uconn, oclass, &conn->object);
		*pobject = &conn->object;
		ret = 0;
	}
	spin_unlock(&disp->client.lock);
	return ret;
}
