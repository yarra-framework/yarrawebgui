build : build-image
	docker run --rm -v `pwd`/../output-${BUILD_OS}:/opt/yarra --mount source=ccache,target=/ccache --env YARRA_BUILD_OS=UBUNTU_${BUILD_OS}04 -it yarraviewer-build:${BUILD_OS}.04

build-image :
	docker build . -f Dockerfile.${BUILD_OS}.build -t yarraviewer-build:${BUILD_OS}.04

update :
	hg pull -R && hg update
