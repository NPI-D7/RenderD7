#include <renderd7/Toast.hpp>

RenderD7::Toast::Toast(std::string head, std::string msg)
{
	this->head = head;
	this->msg = msg;
    this->toast = RenderD7::BitmapPrinter(400, 70);
    this->toast.ClearBlank();
	this->toast.DrawRectFilled(0, 0, 400, 70, 40, 40, 40, 255);
	this->toast.DrawRectFilled(0, 0, 400, 25, 70, 70, 70, 255);
	this->toast.DrawText(4, 5, 0, RenderD7::Color::Hex("#ffffff"), this->head);
	this->toast.DrawText(4, 40, 0, RenderD7::Color::Hex("#ffffff"), this->msg);
	this->toastrendered->LoadPFromBuffer(this->toast.GetBitmap().DATA());
}

void RenderD7::Toast::Draw(void) const
{
	RenderD7::OnScreen(Top);
	/*RenderD7::DrawRect(0, msgposy, 400, 70, RenderD7::Color::Hex("#111111"));
	RenderD7::DrawRect(0, msgposy, 400, 25, RenderD7::Color::Hex("#222222"));
	RenderD7::DrawText(2, msgposy+3, 0.7f, RenderD7::Color::Hex("#ffffff"), head);
	RenderD7::DrawText(2, msgposy+30, 0.6f, RenderD7::Color::Hex("#ffffff"), msg);*/
	toastrendered->Draw(0, 0);
}

void RenderD7::Toast::Logic()
{
	this->delay++/*=1*(int)RenderD7::GetDeltaTime()*/;
	if (msgposy > 170 && delay < 2*60) msgposy--/*=(int)RenderD7::GetDeltaTime()*/;
	
	if (delay >= 5*60)
	{
		msgposy++/*=(int)RenderD7::GetDeltaTime*/;
		if(msgposy > 400) this->Kill();
	}
}