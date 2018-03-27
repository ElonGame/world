#include "Image.h"

#ifdef USE_OPENCV
#include "Interop.h"

#include <stdexcept>
#include <opencv/cv.h>

namespace world {

	inline int getCVType(const ImageType &type) {
		switch (type) {
		case ImageType::RGB :
			return CV_8UC3;
		case ImageType::RGBA :
			return CV_8UC4;
		default :
			return CV_8UC1;
		}
	}
	
	inline ImageType getImageType(int cvType) {
		switch (cvType) {
		case CV_8UC1 :
			return ImageType::GREYSCALE;
		case CV_8UC3:
			return ImageType::RGB;
		case CV_8UC4 :
			return ImageType::RGBA;
		default:
			throw std::runtime_error("Unknown type");
		}
	}

	inline u8 fromFloat(float f) {
		return f >= 1.0 ? (u8) 255 : (f <= 0.0 ? (u8) 0 : (u8) (f * 256.0));
	}

	class PrivateImage {
	public:
		PrivateImage(cv::Mat && image) : _image(image) {}

		cv::Mat _image;
	};

	// Impl�mentation de Pixel

	ConstPixel::ConstPixel(int x, int y, Image * ref)
		: _x(x), _y(y), _ref(ref) {

	}

	Pixel::Pixel(int x, int y, Image * ref) : ConstPixel(x, y, ref) {

	}

	void Pixel::set(u8 r, u8 g, u8 b, u8 a) {
		// TODO am�liorer les performances
		switch (_ref->_type) {
		case ImageType::RGB :
			_ref->_private->_image.at<cv::Vec3b>(_y, _x) = cv::Vec3b(b, g, r);
			break;
		case ImageType::RGBA :
			_ref->_private->_image.at<cv::Vec4b>(_y, _x) = cv::Vec4b(b, g, r, a);
			break;
		default :
			_ref->_private->_image.at<u8>(_y, _x) = r;
		}
	}

	void Pixel::setf(float r, float g, float b, float a) {
		set(fromFloat(r), fromFloat(g), fromFloat(b), fromFloat(a));
	}

	void Pixel::setRed(u8 r) {
		setComponent(2, r);
	}

	void Pixel::setGreen(u8 g) {
		setComponent(1, g);
	}

	void Pixel::setBlue(u8 b) {
		setComponent(0, b);
	}

	void Pixel::setAlpha(u8 a) {
		setComponent(3, a);
	}

	void Pixel::setRedf(float r) {
		setComponent(2, fromFloat(r));
	}

	void Pixel::setGreenf(float r) {
		setComponent(1, fromFloat(r));
	}

	void Pixel::setBluef(float r) {
		setComponent(0, fromFloat(r));
	}

	void Pixel::setAlphaf(float r) {
		setComponent(3, fromFloat(r));
	}

	void Pixel::setLevel(u8 l) {
		setComponent(0, l);
	}

	void Pixel::setLevelf(float l) {
		setComponent(0, fromFloat(l));
	}

	u8 ConstPixel::getAlpha() const {
		return getComponent(3);
	}

	u8 ConstPixel::getRed() const {
		return getComponent(2);
	}

	u8 ConstPixel::getGreen() const {
		return getComponent(1);
	}

	u8 ConstPixel::getBlue() const {
		return getComponent(0);
	}

	u8 ConstPixel::getComponent(int id) const {
		switch (_ref->_type) {
		case ImageType::RGBA:
			return _ref->_private->_image.data[4 * (_ref->_private->_image.cols * _y + _x) + id];
		case ImageType::RGB:
			if (id == 3) return 255;
			return _ref->_private->_image.data[3 * (_ref->_private->_image.cols * _y + _x) + id];
		default:
			if (id == 3) return 255;
			return _ref->_private->_image.data[_ref->_private->_image.cols * _y + _x];
		}
	}

	void Pixel::setComponent(int id, u8 value) {
		switch (_ref->_type) {
		case ImageType::RGB:
			if (id == 3) return;
			_ref->_private->_image.data[3 * (_ref->_private->_image.cols * _y + _x) + id] = value;
			break;
		case ImageType::RGBA:
			_ref->_private->_image.data[4 * (_ref->_private->_image.cols * _y + _x) + id] = value;
			break;
		default:
			if (id == 3) return;
			_ref->_private->_image.data[_ref->_private->_image.cols * _y + _x] = value;
		}
	}



	// Impl�mentation de Image

	Image::Image(int width, int height, const ImageType &type)
		: _private(new PrivateImage(cv::Mat(height, width, getCVType(type)))),
		_type(type) {

	}

	Image::Image(const arma::Cube<double> & data) {
		_private = new PrivateImage(cv::Mat(armaToCV(data)));
		_type = getImageType(_private->_image.type());
	}

	Image::Image(const arma::Mat<double> & data) {
		_private = new PrivateImage(cv::Mat(armaToCV(data)));
		_type = getImageType(_private->_image.type());
	}

	Image::Image(const std::string & filename) {
		_private = new PrivateImage(cv::Mat(cv::imread(filename)));
		_type = getImageType(_private->_image.type());
	}

	Image::Image(Image && image) : _private(image._private), _type(image._type) {
		image._private = nullptr;
	}

	Image::Image(const Image & image) {
		_private = new PrivateImage(cv::Mat(image._private->_image));
		_type = image._type;
	}

	Image::~Image() {
		if (_private != nullptr) // moved
			delete _private;
	}

	ImageType Image::type() const {
		return _type;
	}

	int Image::width() const {
		return _private->_image.cols;
	}

	int Image::height() const {
		return _private->_image.rows;
	}

	Pixel Image::at(int x, int y) {
		return Pixel(x, y, this);
	}

	const ConstPixel Image::at(int x, int y) const {
		return ConstPixel(x, y, const_cast<Image *>(this));
	}

	void Image::write(const std::string &file) {
		cv::imwrite(file, _private->_image);
	}
}

#else // USE_OPENCV

#include <libpng/png.h>

namespace img {
	class PrivateImage {
	public:
		PrivateImage(png_structp img) : _image(img) {}

		png_structp _image;
	};
}

#endif // USE_OPENCV