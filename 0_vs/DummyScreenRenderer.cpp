#include "DummyScreenRenderer.h"

bool DummyScreenRenderer::Initialize(HWND* hWnd) {
	return true;
}

void DummyScreenRenderer::Shutdown(HWND* hWnd) {

}


IImage* DummyScreenRenderer::LoadImage(const TCHAR* filename) {
	return nullptr;
}


void DummyScreenRenderer::BeginDraw() {

}

void DummyScreenRenderer::EndDraw() {

}


void DummyScreenRenderer::DrawImage(int MapId, const RECT* screenPosition, const RECT* imagePosition) {

}

void DummyScreenRenderer::DrawText(const RECT* screenPosition, const TCHAR* string) {

}