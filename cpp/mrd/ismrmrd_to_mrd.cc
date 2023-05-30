#include "generated/binary/protocols.h"
#include <filesystem>
#include <iostream>
#include <exception>
#include <ismrmrd/dataset.h>
#include <ismrmrd/serialization_iostream.h>
#include <xtensor/xview.hpp>

yardl::Date date_from_string(const std::string &s)
{
    std::stringstream ss{s};
    yardl::Date d;
#if __cplusplus < 202002L
    ss >> date::parse("%F", d);
#else
    date::local_days ld;
    ss >> date::parse("%F", ld);
    d = yardl::Date(ld.time_since_epoch());
#endif
    return d;
}

yardl::Time time_from_string(const std::string &s)
{
    std::stringstream ss{s};
    yardl::Time t;
#if __cplusplus < 202002L
    ss >> date::parse("%F", t);
#else
    date::local_days ld;
    ss >> date::parse("%F", ld);
    t = yardl::Date(ld.time_since_epoch());
#endif

    if (ss.fail())
    {
        throw std::runtime_error("invalid date format");
    }
    return t;
};

mrd::SubjectInformationType convert(ISMRMRD::SubjectInformation &subjectInformation)
{
    mrd::SubjectInformationType s;

    if (subjectInformation.patientName)
    {
        s.patient_name = *subjectInformation.patientName;
    }

    if (subjectInformation.patientWeight_kg)
    {
        s.patient_weightkg = *subjectInformation.patientWeight_kg;
    }

    if (subjectInformation.patientHeight_m)
    {
        s.patient_heightm = *subjectInformation.patientHeight_m;
    }

    if (subjectInformation.patientID)
    {
        s.patient_id = *subjectInformation.patientID;
    }

    if (subjectInformation.patientBirthdate)
    {
        s.patient_birthdate = date_from_string(*subjectInformation.patientBirthdate);
    }

    if (subjectInformation.patientGender)
    {
        if (*subjectInformation.patientGender == std::string("M"))
        {
            s.patient_gender = mrd::PatientGender::kM;
        }
        else if (*subjectInformation.patientGender == std::string("F"))
        {
            s.patient_gender = mrd::PatientGender::kF;
        }
        else if (*subjectInformation.patientGender == std::string("O"))
        {
            s.patient_gender = mrd::PatientGender::kO;
        }
        else
        {
            // throw runtime exception
            throw std::runtime_error("Unknown Gender");
        }
    }

    return s;
}

mrd::StudyInformationType convert(ISMRMRD::StudyInformation &studyInformation)
{
    mrd::StudyInformationType s;

    if (studyInformation.studyDate)
    {
        s.study_date = date_from_string(*studyInformation.studyDate);
    }

    if (studyInformation.studyTime)
    {
        s.study_time = time_from_string(*studyInformation.studyTime);
    }

    if (studyInformation.studyID)
    {
        s.study_id = *studyInformation.studyID;
    }

    if (studyInformation.accessionNumber)
    {
        s.accession_number = *studyInformation.accessionNumber;
    }

    if (studyInformation.referringPhysicianName)
    {
        s.referring_physician_name = *studyInformation.referringPhysicianName;
    }

    if (studyInformation.studyDescription)
    {
        s.study_description = *studyInformation.studyDescription;
    }

    if (studyInformation.studyInstanceUID)
    {
        s.study_instance_uid = *studyInformation.studyInstanceUID;
    }

    if (studyInformation.bodyPartExamined)
    {
        s.body_part_examined = *studyInformation.bodyPartExamined;
    }

    return s;
}

mrd::PatientPosition patient_position_from_string(std::string &s)
{
    if (s == "HFP")
    {
        return mrd::PatientPosition::kHFP;
    }
    else if (s == "HFS")
    {
        return mrd::PatientPosition::kHFS;
    }
    else if (s == "HFDR")
    {
        return mrd::PatientPosition::kHFDR;
    }
    else if (s == "HFDL")
    {
        return mrd::PatientPosition::kHFDL;
    }
    else if (s == "FFP")
    {
        return mrd::PatientPosition::kFFP;
    }
    else if (s == "FFS")
    {
        return mrd::PatientPosition::kFFS;
    }
    else if (s == "FFDR")
    {
        return mrd::PatientPosition::kFFDR;
    }
    else if (s == "FFDL")
    {
        return mrd::PatientPosition::kFFDL;
    }
    else
    {
        throw std::runtime_error("Unknown Patient Position");
    }
}

mrd::ThreeDimensionalFloat convert(ISMRMRD::threeDimensionalFloat &threeDimensionalFloat)
{
    mrd::ThreeDimensionalFloat t;
    t.x = threeDimensionalFloat.x;
    t.y = threeDimensionalFloat.y;
    t.z = threeDimensionalFloat.z;
    return t;
}

mrd::MeasurementDependencyType convert(ISMRMRD::MeasurementDependency &measurementDependency)
{
    mrd::MeasurementDependencyType m;
    m.measurement_id = measurementDependency.measurementID;
    m.dependency_type = measurementDependency.dependencyType;
    return m;
}

mrd::MeasurementInformationType convert(ISMRMRD::MeasurementInformation &measurementInformation)
{
    mrd::MeasurementInformationType m;

    if (measurementInformation.measurementID)
    {
        m.measurement_id = *measurementInformation.measurementID;
    }

    if (measurementInformation.seriesDate)
    {
        m.series_date = date_from_string(*measurementInformation.seriesDate);
    }

    if (measurementInformation.seriesTime)
    {
        m.series_time = time_from_string(*measurementInformation.seriesTime);
    }

    m.patient_position = patient_position_from_string(measurementInformation.patientPosition);

    if (measurementInformation.relativeTablePosition)
    {
        m.relative_table_position = convert(*measurementInformation.relativeTablePosition);
    }

    if (measurementInformation.initialSeriesNumber)
    {
        m.initial_series_number = *measurementInformation.initialSeriesNumber;
    }

    if (measurementInformation.protocolName)
    {
        m.protocol_name = *measurementInformation.protocolName;
    }

    if (measurementInformation.sequenceName)
    {
        m.sequence_name = *measurementInformation.sequenceName;
    }

    if (measurementInformation.seriesDescription)
    {
        m.series_description = *measurementInformation.seriesDescription;
    }

    for (auto &dependency : measurementInformation.measurementDependency)
    {
        m.measurement_dependency.push_back(convert(dependency));
    }

    if (measurementInformation.seriesInstanceUIDRoot)
    {
        m.series_instance_uid_root = *measurementInformation.seriesInstanceUIDRoot;
    }

    if (measurementInformation.frameOfReferenceUID)
    {
        m.frame_of_reference_uid = *measurementInformation.frameOfReferenceUID;
    }

    // TODO: Check this, schema is unclear
    if (measurementInformation.referencedImageSequence.size() > 0)
    {
        mrd::ReferencedImageSequenceType referencedImage;
        for (auto &image : measurementInformation.referencedImageSequence)
        {
            referencedImage.referenced_sop_instance_uid.push_back(image.referencedSOPInstanceUID);
        }
        m.referenced_image_sequence = referencedImage;
    }

    if (measurementInformation.referencedImageSequence.size() > 1)
    {
        throw std::runtime_error("Referenced Image Sequence with more than one item is not supported");
    }

    return m;
}

mrd::AcquisitionSystemInformationType convert(ISMRMRD::AcquisitionSystemInformation &a)
{
    mrd::AcquisitionSystemInformationType asi;

    if (a.systemVendor)
    {
        asi.system_vendor = *a.systemVendor;
    }

    if (a.systemModel)
    {
        asi.system_model = *a.systemModel;
    }

    if (a.systemFieldStrength_T)
    {
        asi.system_field_strength_t = *a.systemFieldStrength_T;
    }

    if (a.relativeReceiverNoiseBandwidth)
    {
        asi.relative_receiver_noise_bandwidth = *a.relativeReceiverNoiseBandwidth;
    }

    if (a.receiverChannels)
    {
        asi.receiver_channels = *a.receiverChannels;
    }

    if (a.coilLabel.size() > 0)
    {
        for (auto &c : a.coilLabel)
        {
            mrd::CoilLabelType cl;
            cl.coil_name = c.coilName;
            cl.coil_number = c.coilNumber;
            asi.coil_label.push_back(cl);
        }
    }

    if (a.institutionName)
    {
        asi.institution_name = *a.institutionName;
    }

    if (a.stationName)
    {
        asi.station_name = *a.stationName;
    }

    if (a.deviceID)
    {
        asi.device_id = *a.deviceID;
    }

    if (a.deviceSerialNumber)
    {
        asi.device_serial_number = *a.deviceSerialNumber;
    }

    return asi;
}

mrd::ExperimentalConditionsType convert(ISMRMRD::ExperimentalConditions &e)
{
    mrd::ExperimentalConditionsType ec;
    ec.h1resonance_frequency_hz = e.H1resonanceFrequency_Hz;
    return ec;
}

mrd::MatrixSizeType convert(ISMRMRD::MatrixSize &m)
{
    mrd::MatrixSizeType matrixSize;
    matrixSize.x = m.x;
    matrixSize.y = m.y;
    matrixSize.z = m.z;
    return matrixSize;
}

mrd::FieldOfViewmm convert(ISMRMRD::FieldOfView_mm &f)
{
    mrd::FieldOfViewmm fieldOfView;
    fieldOfView.x = f.x;
    fieldOfView.y = f.y;
    fieldOfView.z = f.z;
    return fieldOfView;
}

mrd::EncodingSpaceType convert(ISMRMRD::EncodingSpace &e)
{
    mrd::EncodingSpaceType encodingSpace;
    encodingSpace.matrix_size = convert(e.matrixSize);
    encodingSpace.field_of_viewmm = convert(e.fieldOfView_mm);
    return encodingSpace;
}

mrd::LimitType convert(ISMRMRD::Limit &l)
{
    mrd::LimitType limit;
    limit.minimum = l.minimum;
    limit.maximum = l.maximum;
    limit.center = l.center;
    return limit;
}

mrd::EncodingLimitsType convert(ISMRMRD::EncodingLimits &e)
{
    mrd::EncodingLimitsType encodingLimits;

    if (e.kspace_encoding_step_0)
    {
        encodingLimits.kspaceencodingstep0 = convert(*e.kspace_encoding_step_0);
    }

    if (e.kspace_encoding_step_1)
    {
        encodingLimits.kspaceencodingstep1 = convert(*e.kspace_encoding_step_1);
    }

    if (e.kspace_encoding_step_2)
    {
        encodingLimits.kspaceencodingstep2 = convert(*e.kspace_encoding_step_2);
    }

    if (e.average)
    {
        encodingLimits.average = convert(*e.average);
    }

    if (e.slice)
    {
        encodingLimits.slice = convert(*e.slice);
    }

    if (e.contrast)
    {
        encodingLimits.contrast = convert(*e.contrast);
    }

    if (e.phase)
    {
        encodingLimits.phase = convert(*e.phase);
    }

    if (e.repetition)
    {
        encodingLimits.repetition = convert(*e.repetition);
    }

    if (e.set)
    {
        encodingLimits.set = convert(*e.set);
    }

    if (e.segment)
    {
        encodingLimits.segment = convert(*e.segment);
    }

    if (e.user[0])
    {
        encodingLimits.user0 = convert(*e.user[0]);
    }

    if (e.user[1])
    {
        encodingLimits.user1 = convert(*e.user[1]);
    }

    if (e.user[2])
    {
        encodingLimits.user2 = convert(*e.user[2]);
    }

    if (e.user[3])
    {
        encodingLimits.user3 = convert(*e.user[3]);
    }

    if (e.user[4])
    {
        encodingLimits.user4 = convert(*e.user[4]);
    }

    if (e.user[5])
    {
        encodingLimits.user5 = convert(*e.user[5]);
    }

    if (e.user[6])
    {
        encodingLimits.user6 = convert(*e.user[6]);
    }

    if (e.user[7])
    {
        encodingLimits.user7 = convert(*e.user[7]);
    }

    return encodingLimits;
}

mrd::UserParameterLongType convert(ISMRMRD::UserParameterLong &u)
{
    mrd::UserParameterLongType userParameterLong;
    userParameterLong.name = u.name;
    userParameterLong.value = u.value;
    return userParameterLong;
}

mrd::UserParameterDoubleType convert(ISMRMRD::UserParameterDouble &u)
{
    mrd::UserParameterDoubleType userParameterDouble;
    userParameterDouble.name = u.name;
    userParameterDouble.value = u.value;
    return userParameterDouble;
}

mrd::UserParameterStringType convert(ISMRMRD::UserParameterString &u)
{
    mrd::UserParameterStringType userParameterString;
    userParameterString.name = u.name;
    userParameterString.value = u.value;
    return userParameterString;
}

mrd::TrajectoryDescriptionType convert(ISMRMRD::TrajectoryDescription &t)
{
    mrd::TrajectoryDescriptionType trajectoryDescription;
    trajectoryDescription.identifier = t.identifier;

    for (auto &u : t.userParameterLong)
    {
        trajectoryDescription.user_parameter_long.push_back(convert(u));
    }

    for (auto &u : t.userParameterDouble)
    {
        trajectoryDescription.user_parameter_double.push_back(convert(u));
    }

    for (auto &u : t.userParameterString)
    {
        trajectoryDescription.user_parameter_string.push_back(convert(u));
    }

    if (t.comment)
    {
        trajectoryDescription.comment = *t.comment;
    }

    return trajectoryDescription;
}

mrd::AccelerationFactorType convert(ISMRMRD::AccelerationFactor &a)
{
    mrd::AccelerationFactorType accelerationFactor;
    accelerationFactor.kspaceencodingstep1 = a.kspace_encoding_step_1;
    accelerationFactor.kspaceencodingstep2 = a.kspace_encoding_step_2;
    return accelerationFactor;
}

mrd::CalibrationMode calibration_mode_from_string(std::string &m)
{
    if (m == "embedded")
    {
        return mrd::CalibrationMode::kEmbedded;
    }
    else if (m == "interleaved")
    {
        return mrd::CalibrationMode::kInterleaved;
    }
    else if (m == "separate")
    {
        return mrd::CalibrationMode::kSeparate;
    }
    else if (m == "external")
    {
        return mrd::CalibrationMode::kExternal;
    }
    else if (m == "other")
    {
        return mrd::CalibrationMode::kOther;
    }
    else
    {
        throw std::runtime_error("Unknown CalibrationMode: " + m);
    }
}

mrd::InterleavingDimension interleaving_dimension_from_string(std::string &s)
{
    if (s == "phase")
    {
        return mrd::InterleavingDimension::kPhase;
    }
    else if (s == "repetition")
    {
        return mrd::InterleavingDimension::kRepetition;
    }
    else if (s == "contrast")
    {
        return mrd::InterleavingDimension::kContrast;
    }
    else if (s == "average")
    {
        return mrd::InterleavingDimension::kAverage;
    }
    else if (s == "other")
    {
        return mrd::InterleavingDimension::kOther;
    }
    else
    {
        throw std::runtime_error("Unknown InterleavingDimension: " + s);
    }
}

mrd::MultibandSpacingType convert(ISMRMRD::MultibandSpacing &m)
{
    mrd::MultibandSpacingType multibandSpacing;
    for (auto s : m.dZ)
    {
        multibandSpacing.d_z.push_back(s);
    }
    return multibandSpacing;
}

mrd::Calibration convert(ISMRMRD::MultibandCalibrationType &m)
{
    if (m == ISMRMRD::MultibandCalibrationType::SEPARABLE2D)
    {
        return mrd::Calibration::kSeparable2D;
    }
    else if (m == ISMRMRD::MultibandCalibrationType::FULL3D)
    {
        return mrd::Calibration::kFull3D;
    }
    else if (m == ISMRMRD::MultibandCalibrationType::OTHER)
    {
        return mrd::Calibration::kOther;
    }
    else
    {
        throw std::runtime_error("Unknown Calibration");
    }
}

mrd::MultibandType convert(ISMRMRD::Multiband &m)
{
    mrd::MultibandType multiband;
    for (auto s : m.spacing)
    {
        multiband.spacing.push_back(convert(s));
    }
    multiband.delta_kz = m.deltaKz;
    multiband.multibandfactor = m.multiband_factor;
    multiband.calibration = convert(m.calibration);
    multiband.calibrationencoding = m.calibration_encoding;
    return multiband;
}

mrd::ParallelImagingType convert(ISMRMRD::ParallelImaging &p)
{
    mrd::ParallelImagingType parallelImaging;
    parallelImaging.acceleration_factor = convert(p.accelerationFactor);
    if (p.calibrationMode)
    {
        parallelImaging.calibration_mode = calibration_mode_from_string(*p.calibrationMode);
    }
    if (p.interleavingDimension)
    {
        parallelImaging.interleaving_dimension = interleaving_dimension_from_string(*p.interleavingDimension);
    }
    if (p.multiband)
    {
        parallelImaging.multiband = convert(*p.multiband);
    }
    return parallelImaging;
}

mrd::EncodingType convert(ISMRMRD::Encoding &e)
{
    mrd::EncodingType encoding;

    encoding.encoded_space = convert(e.encodedSpace);
    encoding.recon_space = convert(e.reconSpace);
    encoding.encoding_limits = convert(e.encodingLimits);

    if (e.trajectory == ISMRMRD::TrajectoryType::CARTESIAN)
    {
        encoding.trajectory = mrd::Trajectory::kCartesian;
    }
    else if (e.trajectory == ISMRMRD::TrajectoryType::EPI)
    {
        encoding.trajectory = mrd::Trajectory::kEpi;
    }
    else if (e.trajectory == ISMRMRD::TrajectoryType::RADIAL)
    {
        encoding.trajectory = mrd::Trajectory::kRadial;
    }
    else if (e.trajectory == ISMRMRD::TrajectoryType::GOLDENANGLE)
    {
        encoding.trajectory = mrd::Trajectory::kGoldenangle;
    }
    else if (e.trajectory == ISMRMRD::TrajectoryType::SPIRAL)
    {
        encoding.trajectory = mrd::Trajectory::kSpiral;
    }
    else if (e.trajectory == ISMRMRD::TrajectoryType::OTHER)
    {
        encoding.trajectory = mrd::Trajectory::kOther;
    }
    else
    {
        // throw error
        throw std::runtime_error("Unknown trajectory type");
    }

    if (e.trajectoryDescription)
    {
        encoding.trajectory_description = convert(*e.trajectoryDescription);
    }

    if (e.parallelImaging)
    {
        encoding.parallel_imaging = convert(*e.parallelImaging);
    }

    if (e.echoTrainLength)
    {
        encoding.echo_train_length = *e.echoTrainLength;
    }

    return encoding;
}

mrd::DiffusionDimension convert(ISMRMRD::DiffusionDimension &diffusionDimension)
{
    if (diffusionDimension == ISMRMRD::DiffusionDimension::AVERAGE)
    {
        return mrd::DiffusionDimension::kAverage;
    }
    else if (diffusionDimension == ISMRMRD::DiffusionDimension::CONTRAST)
    {
        return mrd::DiffusionDimension::kContrast;
    }
    else if (diffusionDimension == ISMRMRD::DiffusionDimension::PHASE)
    {
        return mrd::DiffusionDimension::kPhase;
    }
    else if (diffusionDimension == ISMRMRD::DiffusionDimension::REPETITION)
    {
        return mrd::DiffusionDimension::kRepetition;
    }
    else if (diffusionDimension == ISMRMRD::DiffusionDimension::SET)
    {
        return mrd::DiffusionDimension::kSet;
    }
    else if (diffusionDimension == ISMRMRD::DiffusionDimension::SEGMENT)
    {
        return mrd::DiffusionDimension::kSegment;
    }
    else if (diffusionDimension == ISMRMRD::DiffusionDimension::USER_0)
    {
        return mrd::DiffusionDimension::kUser0;
    }
    else if (diffusionDimension == ISMRMRD::DiffusionDimension::USER_1)
    {
        return mrd::DiffusionDimension::kUser1;
    }
    else if (diffusionDimension == ISMRMRD::DiffusionDimension::USER_2)
    {
        return mrd::DiffusionDimension::kUser2;
    }
    else if (diffusionDimension == ISMRMRD::DiffusionDimension::USER_3)
    {
        return mrd::DiffusionDimension::kUser3;
    }
    else if (diffusionDimension == ISMRMRD::DiffusionDimension::USER_4)
    {
        return mrd::DiffusionDimension::kUser4;
    }
    else if (diffusionDimension == ISMRMRD::DiffusionDimension::USER_5)
    {
        return mrd::DiffusionDimension::kUser5;
    }
    else if (diffusionDimension == ISMRMRD::DiffusionDimension::USER_6)
    {
        return mrd::DiffusionDimension::kUser6;
    }
    else if (diffusionDimension == ISMRMRD::DiffusionDimension::USER_7)
    {
        return mrd::DiffusionDimension::kUser7;
    }
    else
    {
        throw std::runtime_error("Unknown DiffusionDimension");
    }
}

mrd::GradientDirectionType convert(ISMRMRD::GradientDirection &g)
{
    mrd::GradientDirectionType gradientDirection;
    gradientDirection.rl = g.rl;
    gradientDirection.ap = g.ap;
    gradientDirection.fh = g.fh;
    return gradientDirection;
}

mrd::DiffusionType convert(ISMRMRD::Diffusion &d)
{
    mrd::DiffusionType diffusion;
    diffusion.gradient_direction = convert(d.gradientDirection);
    diffusion.bvalue = d.bvalue;
    return diffusion;
}

mrd::SequenceParametersType convert(ISMRMRD::SequenceParameters &s)
{
    mrd::SequenceParametersType sequenceParameters;

    if (s.TR)
    {
        for (auto &t : *s.TR)
        {
            sequenceParameters.t_r.push_back(t);
        }
    }

    if (s.TE)
    {
        for (auto &t : *s.TE)
        {
            sequenceParameters.t_e.push_back(t);
        }
    }

    if (s.TI)
    {
        for (auto &t : *s.TI)
        {
            sequenceParameters.t_i.push_back(t);
        }
    }

    if (s.flipAngle_deg)
    {
        for (auto &t : *s.flipAngle_deg)
        {
            sequenceParameters.flip_angledeg.push_back(t);
        }
    }

    if (s.sequence_type)
    {
        sequenceParameters.sequencetype = *s.sequence_type;
    }

    if (s.echo_spacing)
    {
        for (auto &t : *s.echo_spacing)
        {
            sequenceParameters.echospacing.push_back(t);
        }
    }

    if (s.diffusionDimension)
    {
        sequenceParameters.diffusion_dimension = convert(*s.diffusionDimension);
    }

    if (s.diffusion)
    {
        for (auto &d : *s.diffusion)
        {
            sequenceParameters.diffusion.push_back(convert(d));
        }
    }

    if (s.diffusionScheme)
    {
        sequenceParameters.diffusion_scheme = *s.diffusionScheme;
    }

    return sequenceParameters;
}

mrd::UserParameterBase64Type convert_userbase64(ISMRMRD::UserParameterString &u)
{
    mrd::UserParameterBase64Type userParameterBase64;
    userParameterBase64.name = u.name;
    userParameterBase64.value = u.value;
    return userParameterBase64;
}

mrd::UserParametersType convert(ISMRMRD::UserParameters &u)
{
    mrd::UserParametersType userParameters;

    for (auto &p : u.userParameterLong)
    {
        userParameters.user_parameter_long.push_back(convert(p));
    }

    for (auto &p : u.userParameterDouble)
    {
        userParameters.user_parameter_double.push_back(convert(p));
    }

    for (auto &p : u.userParameterString)
    {
        userParameters.user_parameter_string.push_back(convert(p));
    }

    for (auto &p : u.userParameterBase64)
    {
        userParameters.user_parameter_base64.push_back(convert_userbase64(p));
    }

    return userParameters;
}

mrd::WaveformType convert(ISMRMRD::WaveformType &w)
{
    if (w == ISMRMRD::WaveformType::ECG)
    {
        return mrd::WaveformType::kEcg;
    }
    else if (w == ISMRMRD::WaveformType::PULSE)
    {
        return mrd::WaveformType::kPulse;
    }
    else if (w == ISMRMRD::WaveformType::RESPIRATORY)
    {
        return mrd::WaveformType::kRespiratory;
    }
    else if (w == ISMRMRD::WaveformType::TRIGGER)
    {
        return mrd::WaveformType::kTrigger;
    }
    else if (w == ISMRMRD::WaveformType::GRADIENTWAVEFORM)
    {
        return mrd::WaveformType::kGradientwaveform;
    }
    else if (w == ISMRMRD::WaveformType::OTHER)
    {
        return mrd::WaveformType::kOther;
    }
    else
    {
        throw std::runtime_error("Unknown WaveformType");
    }
}

mrd::WaveformInformationType convert(ISMRMRD::WaveformInformation &w)
{
    mrd::WaveformInformationType waveformInformation;
    waveformInformation.waveform_name = w.waveformName;
    waveformInformation.waveform_type = convert(w.waveformType);
    if (w.userParameters)
    {
        waveformInformation.user_parameters = convert(*w.userParameters);
    }
    return waveformInformation;
}

mrd::Header convert(ISMRMRD::IsmrmrdHeader &hdr)
{
    mrd::Header h;

    if (hdr.version)
    {
        h.version = *hdr.version;
    }

    if (hdr.subjectInformation)
    {
        h.subject_information = convert(*hdr.subjectInformation);
    }

    if (hdr.studyInformation)
    {
        h.study_information = convert(*hdr.studyInformation);
    }

    if (hdr.measurementInformation)
    {
        h.measurement_information = convert(*hdr.measurementInformation);
    }

    if (hdr.acquisitionSystemInformation)
    {
        h.acquisition_system_information = convert(*hdr.acquisitionSystemInformation);
    }

    h.experimental_conditions = convert(hdr.experimentalConditions);

    if (hdr.encoding.size() > 0)
    {
        for (auto e : hdr.encoding)
        {
            h.encoding.push_back(convert(e));
        }
    }
    else
    {
        throw std::runtime_error("No encoding found in ISMRMRD header");
    }

    if (hdr.sequenceParameters)
    {
        h.sequence_parameters = convert(*hdr.sequenceParameters);
    }

    if (hdr.userParameters)
    {
        h.user_parameters = convert(*hdr.userParameters);
    }

    for (auto w : hdr.waveformInformation)
    {
        h.waveform_information.push_back(convert(w));
    }

    return h;
}

mrd::EncodingCounters convert(ISMRMRD::EncodingCounters &e)
{
    mrd::EncodingCounters encodingCounters;
    encodingCounters.kspace_encode_step1 = e.kspace_encode_step_1;
    encodingCounters.kspace_encode_step2 = e.kspace_encode_step_2;
    encodingCounters.average = e.average;
    encodingCounters.slice = e.slice;
    encodingCounters.contrast = e.contrast;
    encodingCounters.phase = e.phase;
    encodingCounters.repetition = e.repetition;
    encodingCounters.set = e.set;
    encodingCounters.segment = e.segment;
    for (auto &u : e.user)
    {
        encodingCounters.user.push_back(u);
    }

    return encodingCounters;
}

mrd::Acquisition convert(ISMRMRD::Acquisition &acq)
{
    mrd::Acquisition acquisition;

    acquisition.flags = acq.flags();
    acquisition.idx = convert(acq.idx());
    acquisition.measurement_uid = acq.measurement_uid();
    acquisition.scan_counter = acq.scan_counter();
    acquisition.acquisition_time_stamp = acq.acquisition_time_stamp();
    for (auto &p : acq.physiology_time_stamp())
    {
        acquisition.physiology_time_stamp.push_back(p);
    }

    // TODO: Convert channel_mask to channel_order
    acquisition.discard_pre = acq.discard_pre();
    acquisition.discard_post = acq.discard_post();
    acquisition.center_sample = acq.center_sample();
    acquisition.encoding_space_ref = acq.encoding_space_ref();
    acquisition.sample_time_us = acq.sample_time_us();

    acquisition.position[0] = acq.position()[0];
    acquisition.position[1] = acq.position()[1];
    acquisition.position[2] = acq.position()[2];

    acquisition.read_dir[0] = acq.read_dir()[0];
    acquisition.read_dir[1] = acq.read_dir()[1];
    acquisition.read_dir[2] = acq.read_dir()[2];

    acquisition.phase_dir[0] = acq.phase_dir()[0];
    acquisition.phase_dir[1] = acq.phase_dir()[1];
    acquisition.phase_dir[2] = acq.phase_dir()[2];

    acquisition.slice_dir[0] = acq.slice_dir()[0];
    acquisition.slice_dir[1] = acq.slice_dir()[1];
    acquisition.slice_dir[2] = acq.slice_dir()[2];

    acquisition.patient_table_position[0] = acq.patient_table_position()[0];
    acquisition.patient_table_position[1] = acq.patient_table_position()[1];
    acquisition.patient_table_position[2] = acq.patient_table_position()[2];

    for (auto &p : acq.user_int())
    {
        acquisition.user_int.push_back(p);
    }
    for (auto &p : acq.user_float())
    {
        acquisition.user_float.push_back(p);
    }

    mrd::AcquisitionData data({acq.active_channels(), acq.number_of_samples()});
    for (uint16_t c = 0; c < acq.active_channels(); c++)
    {
        for (uint16_t s = 0; s < acq.number_of_samples(); s++)
        {
            data(c, s) = acq.data(s, c);
        }
    }

    acquisition.data = xt::view(data, xt::all(), xt::all());

    if (acq.trajectory_dimensions() > 0)
    {
        mrd::TrajectoryData trajectory({acq.trajectory_dimensions(), acq.number_of_samples()});
        for (uint16_t d = 0; d < acq.trajectory_dimensions(); d++)
        {
            for (uint16_t s = 0; s < acq.number_of_samples(); s++)
            {
                trajectory(d, s) = acq.traj(s, d);
            }
        }
        acquisition.trajectory = xt::view(trajectory, xt::all(), xt::all());
    }

    return acquisition;
}

// Convert ISMRMRD::Image<T> to mrd::Image<T>
// Schema:
// Image<T>: !record
//   fields:
//     flags: uint64
//     measurementUid: uint #Remove?
//     fieldOfView: !array
//       items: float
//       dimensions: [3]
//     position: !array
//       items: float
//       dimensions: [3]
//     colDir: !array
//       items: float
//       dimensions: [3]
//     lineDir: !array
//       items: float
//       dimensions: [3]
//     sliceDir: !array
//       items: float
//       dimensions: [3]
//     patientTablePosition: !array
//       items: float
//       dimensions: [3]
//     average: uint?
//     slice: uint?
//     contrast: uint?
//     phase: uint
//     repetition: uint?
//     set: uint?
//     acquisitionTimeStamp: uint?
//     physiologyTimeStamp: !array
//       items: uint
//       dimensions: [3]
//     imageType: ImageType
//     imageIndex: uint?
//     imageSeriesIndex: uint?
//     userInt: !vector
//       items: int
//     userFloat: !vector
//       items: float
//     data: ImageData<T>
//     meta: string->string
//   computedFields:
//     channels: size(data, "channel")
//     slices: size(data, "z")
//     rows: size(data, "y")
//     cols: size(data, "x")
template <typename T>
mrd::Image<T> convert(ISMRMRD::Image<T> &im)
{
    mrd::Image<T> image;
    image.flags = im.getFlags();
    image.measurement_uid = im.getMeasurementUid();
    image.field_of_view[0] = im.getFieldOfViewX();
    image.field_of_view[1] = im.getFieldOfViewY();
    image.field_of_view[2] = im.getFieldOfViewZ();
    image.position[0] = im.getPositionX();
    image.position[1] = im.getPositionY();
    image.position[2] = im.getPositionZ();
    image.col_dir[0] = im.getReadDirectionX();
    image.col_dir[1] = im.getReadDirectionY();
    image.col_dir[2] = im.getReadDirectionZ();
    image.line_dir[0] = im.getPhaseDirectionX();
    image.line_dir[1] = im.getPhaseDirectionY();
    image.line_dir[2] = im.getPhaseDirectionZ();
    image.slice_dir[0] = im.getSliceDirectionX();
    image.slice_dir[1] = im.getSliceDirectionY();
    image.slice_dir[2] = im.getSliceDirectionZ();
    image.patient_table_position[0] = im.getPatientTablePositionX();
    image.patient_table_position[1] = im.getPatientTablePositionY();
    image.patient_table_position[2] = im.getPatientTablePositionZ();
    image.average = im.getAverage();
    image.slice = im.getSlice();
    image.contrast = im.getContrast();
    image.phase = im.getPhase();
    image.repetition = im.getRepetition();
    image.set = im.getSet();
    image.acquisition_time_stamp = im.getAcquisitionTimeStamp();
    image.physiology_time_stamp[0] = im.getPhysiologyTimeStamp(0);
    image.physiology_time_stamp[1] = im.getPhysiologyTimeStamp(1);
    image.physiology_time_stamp[2] = im.getPhysiologyTimeStamp(2);

    if (im.getImageType() == ISMRMRD::ISMRMRD_ImageTypes::ISMRMRD_IMTYPE_COMPLEX)
    {
        image.image_type = mrd::ImageType::kComplex;
    }
    else if (im.getImageType() == ISMRMRD::ISMRMRD_ImageTypes::ISMRMRD_IMTYPE_MAGNITUDE)
    {
        image.image_type = mrd::ImageType::kMagnitude;
    }
    else if (im.getImageType() == ISMRMRD::ISMRMRD_ImageTypes::ISMRMRD_IMTYPE_REAL)
    {
        image.image_type = mrd::ImageType::kReal;
    }
    else if (im.getImageType() == ISMRMRD::ISMRMRD_ImageTypes::ISMRMRD_IMTYPE_PHASE)
    {
        image.image_type = mrd::ImageType::kPhase;
    }
    else if (im.getImageType() == ISMRMRD::ISMRMRD_ImageTypes::ISMRMRD_IMTYPE_IMAG)
    {
        image.image_type = mrd::ImageType::kImag;
    }
    else
    {
        throw std::runtime_error("Unknown image type");
    }

    image.image_index = im.getImageIndex();
    image.image_series_index = im.getImageSeriesIndex();
    for (int i = 0; i < ISMRMRD::ISMRMRD_USER_INTS; i++)
    {
        image.user_int.push_back(im.getUserInt(i));
    }
    for (int i = 0; i < ISMRMRD::ISMRMRD_USER_FLOATS; i++)
    {
        image.user_float.push_back(im.getUserFloat(i));
    }
    // image.data = convert(im.data());
    // image.meta = im.meta();
    return image;
}

int main()
{
    ISMRMRD::IStreamView rs(std::cin);
    ISMRMRD::ProtocolDeserializer deserializer(rs);
    mrd::binary::MrdWriter w(std::cout);

    // Some reconstructions return the header but it is not required.
    if (deserializer.peek() == ISMRMRD::ISMRMRD_MESSAGE_HEADER)
    {
        ISMRMRD::IsmrmrdHeader hdr;
        deserializer.deserialize(hdr);
        w.WriteHeader(convert(hdr));
    }
    else
    {
        w.WriteHeader(std::nullopt);
    }

    while (deserializer.peek() != ISMRMRD::ISMRMRD_MESSAGE_CLOSE)
    {
        if (deserializer.peek() == ISMRMRD::ISMRMRD_MESSAGE_ACQUISITION)
        {
            ISMRMRD::Acquisition acq;
            deserializer.deserialize(acq);
            w.WriteData(convert(acq));
        }
        else if (deserializer.peek() == ISMRMRD::ISMRMRD_MESSAGE_IMAGE)
        {
            if (deserializer.peek_image_data_type() == ISMRMRD::ISMRMRD_USHORT)
            {
                ISMRMRD::Image<unsigned short> img;
                deserializer.deserialize(img);

                std::cerr << "USHORT IMAGE ENCOUNTERED" << std::endl;
                // Convert Image
            }
            else if (deserializer.peek_image_data_type() == ISMRMRD::ISMRMRD_SHORT)
            {
                ISMRMRD::Image<short> img;
                deserializer.deserialize(img);

                // Convert Image
            }
            else if (deserializer.peek_image_data_type() == ISMRMRD::ISMRMRD_UINT)
            {
                ISMRMRD::Image<unsigned int> img;
                deserializer.deserialize(img);

                // Convert Image
            }
            else if (deserializer.peek_image_data_type() == ISMRMRD::ISMRMRD_INT)
            {
                ISMRMRD::Image<int> img;
                deserializer.deserialize(img);

                // Convert Image
            }
            else if (deserializer.peek_image_data_type() == ISMRMRD::ISMRMRD_FLOAT)
            {
                ISMRMRD::Image<float> img;
                deserializer.deserialize(img);

                auto image = convert(img);
                std::cerr << "FLOAT IMAGE ENCOUNTERED" << std::endl;
                // Convert Image
            }
            else if (deserializer.peek_image_data_type() == ISMRMRD::ISMRMRD_DOUBLE)
            {
                ISMRMRD::Image<double> img;
                deserializer.deserialize(img);

                // Convert Image
            }
            else if (deserializer.peek_image_data_type() == ISMRMRD::ISMRMRD_CXFLOAT)
            {
                ISMRMRD::Image<std::complex<float>> img;
                deserializer.deserialize(img);

                // Convert Image
            }
            else if (deserializer.peek_image_data_type() == ISMRMRD::ISMRMRD_CXDOUBLE)
            {
                ISMRMRD::Image<std::complex<double>> img;
                deserializer.deserialize(img);

                // Convert Image
            }
            else
            {
                throw std::runtime_error("Unknown image type");
            }
        }
        else if (deserializer.peek() == ISMRMRD::ISMRMRD_MESSAGE_WAVEFORM)
        {
            ISMRMRD::Waveform wfm;
            deserializer.deserialize(wfm);

            // Convert Waveform
        }
        else
        {
            std::cerr << "Unexpected ISMRMRD message type: " << deserializer.peek() << std::endl;
            return 1;
        }
    }

    w.EndData();

    return 0;
}