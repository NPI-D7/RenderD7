#include <renderd7/Color.hpp>

#define RGBA8(r, g, b, a) ((((r) & 0xFF) << 0) | (((g) & 0xFF) << 8) | (((b) & 0xFF) << 16) | (((a) & 0xFF) << 24))

uint32_t RenderD7::Color::Hex(const std::string color, uint8_t a)
{
    if (color.length() < 7 || std::regex_search(color.substr(1), std::regex("[^0-9A-Fa-f]"))) { // invalid color.
		return RenderD7::Color::Hex("#000000", 0);
	}
    int r = std::stoi(color.substr(1, 2), nullptr, 16);
	int g = std::stoi(color.substr(3, 2), nullptr, 16);
	int b = std::stoi(color.substr(5, 2), nullptr, 16);
	return RGBA8(r, g, b, a);
}

std::string RenderD7::Color::RGB2Hex(int r, int g, int b)
{
  	std::stringstream ss;
  	ss << "#";
  	ss << std::hex << (r << 16 | g << 8 | b );
  	return ss.str();
}

uint32_t RenderD7::Color::Convert(uint32_t src, RenderD7::Color::ColorFmt srcFormat, RenderD7::Color::ColorFmt dstFormat)
{
    uint32_t result = 0x00000000;
    //uint8_t red, green, blue, alpha = 0x00;
    /*switch(srcFormat)
    {
        case RGBA8:
        {
            red, green, blue, alpha = (UNPACK_RGBA(src));
            switch (dstFormat)
            {
            case RGBA8:
            {
                result = ((((red) & 0xFF) << 0) | (((green) & 0xFF) << 8) | (((blue) & 0xFF) << 16) | (((alpha) & 0xFF) << 24));
                break;
            }
            case RGB8:
            {
                result = ((((red) & 0xFF) << 0) | (((green) & 0xFF) << 8) | (((blue) & 0xFF) << 16));
                break;
            }
            case RGB565:
            {
                //NOTYET
                break;
            }
            case BGRA8:
            {
                result = ((((blue)  & 0xFF) << 16) | (((green) & 0xFF) << 8) | (((red) & 0xFF) << 0 | (((alpha) & 0xFF) << 24)));
                break;
            }
            case BGR8:
            {
                result = ((((blue) & 0xFF) << 16) | (((green) & 0xFF) << 8) | (((red) & 0xFF) << 0));
                break;
            }
                
            default:
                break;
            }
            
            break;
        }
        case RGB8:
        {
            result[2] = src[2];
            result[1] = src[1];
            result[0] = src[0];
            break;
        }
        case RGB565:
        {
            // thanks neobrain
            uint16_t px = *(uint16_t *)src;
            blue = px & 0x1F;
            green = (px >> 5) & 0x3F;
            red = (px >> 11) & 0x1F;

            result[0] = (blue  << 3) | (blue  >> 2);
            result[1] = (green << 2) | (green >> 4);
            result[2] = (red   << 3) | (red   >> 2);

            break;
        }
        
        default: break;
    }*/
    return result;
}