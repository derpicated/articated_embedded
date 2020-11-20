// algorithm_random.hpp

#ifndef ALGORITHM_RANDOM_HPP
#define ALGORITHM_RANDOM_HPP

#include <QMutex>
#include <QVideoFrame>

#include "../vision_algorithm.hpp"

class AlgorithmRandom : public VisionAlgorithm {
    public:
    AlgorithmRandom ();
    ~AlgorithmRandom ();

    void SetReference ();
    FrameData Execute (const QVideoFrame& const_buffer);

    private:
    QMutex movement_mutex_;
    Movement3D last_movement_;
    Movement3D random_movement_;
};

#endif // ALGORITHM_RANDOM_HPP
