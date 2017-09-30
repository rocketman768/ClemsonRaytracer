OBJS = image.o main.o ellipsoid.o model.o parsefile.o ppm_tools.o projection.o ray.o \
	sphere.o sphere_pixmap.o list.o material.o object.o plane.o sceneobj.o veclib.o \
	light.o illuminate.o fplane.o fplane_tex.o
OPTS = -g -Wall
OPTMZ = -O3
PROFILE = -pg
LIBS = -lm
INCS = ppm_tools.h ray.h rayhdrs.h veclib.h

all: ray3 
	
ray3: $(OBJS)
	gcc $(OBJS) -o ray3 $(LIBS)

$(OBJS): $(INCS) Makefile

.c.o: $<
	gcc $(OPTS) $(OPTMZ) -c $<

clean:
	-rm -vf *.o ray3

