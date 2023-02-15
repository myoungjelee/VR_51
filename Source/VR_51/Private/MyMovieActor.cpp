// Fill out your copyright notice in the Description page of Project Settings.


#include "MyMovieActor.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "MediaSource.h"
#include "MediaPlayer.h"
#include "MediaSoundComponent.h"
#include "Components/WidgetComponent.h"
#include "MovieWidget.h"


// Sets default values
AMyMovieActor::AMyMovieActor()
{
	PrimaryActorTick.bCanEverTick = true;

	rootComp = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(rootComp);

	screenComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Screen"));
	screenComp->SetupAttachment(RootComponent);

	soundComp = CreateDefaultSubobject<UMediaSoundComponent>(TEXT("Sounds"));
	soundComp->SetupAttachment(RootComponent);

	playback_widget = CreateDefaultSubobject<UWidgetComponent>(TEXT("Widget Component"));
	playback_widget->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AMyMovieActor::BeginPlay()
{
	Super::BeginPlay();

	if (playback_widget != nullptr)
	{
		UMovieWidget* movieWidget = Cast<UMovieWidget>(playback_widget->GetWidget());

		if (movieWidget != nullptr)
		{
			movieWidget->movieActor = this;
		}
	}

	PlayMovie();
}

// Called every frame
void AMyMovieActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMyMovieActor::PlayMovie()
{
	UE_LOG(LogTemp, Log, TEXT("Play Movie Function!!!!!!!!!"));

	// 미디어 플레이 테스트
	for (int32 i = 0; i < mediaPlayer.Num(); i++)
	{
		if (videoSouce[i] != nullptr && mediaPlayer[i] != nullptr)
		{
			if (mediaPlayer[i]->IsPlaying())
			{
				mediaPlayer[i]->Pause();
			}
			else
			{
				if (mediaPlayer[i]->IsPaused())
				{
					mediaPlayer[i]->Play();
				}
				// 미디어 플레이어에서 실행할 비디오 소스 파일을 연다.
				else
				{
					mediaPlayer[i]->OpenSource(videoSouce[i]);
					mediaPlayer[i]->Play();
				}

			}
		}
	}
}

void AMyMovieActor::StopMovie()
{

	if (mediaPlayer[0] != nullptr)
	{
		mediaPlayer[0]->Close();
	}
}

void AMyMovieActor::ReverseMovie(float second)
{
	if (mediaPlayer[0]->IsPlaying())
	{
		FTimespan modifiedTime = mediaPlayer[0]->GetTime() - FTimespan(0, 0, second);
		mediaPlayer[0]->Seek(modifiedTime);
	}
}

void AMyMovieActor::ForwardMovie(float second)
{
	if (mediaPlayer[0]->IsPlaying())
	{
		FTimespan modifiedTime = mediaPlayer[0]->GetTime() + FTimespan(0, 0, second);
		mediaPlayer[0]->Seek(modifiedTime);
	}
}

void AMyMovieActor::ChangeMovie()
{
	// videoSource의 0번과 1번을 계속 번갈아가며 변경한다.
	FTimespan currentPlayTime = mediaPlayer[0]->GetTime();
	
	videoNum = ++videoNum % 2;
	mediaPlayer[0]->OpenSource(videoSouce[videoNum]);
	mediaPlayer[0]->Seek(currentPlayTime);
}

