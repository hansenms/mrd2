#include "generated/binary/protocols.h"
#include <filesystem>
#include <iostream>
#include <exception>
#include <ismrmrd/dataset.h>
#include <ismrmrd/serialization_iostream.h>

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

    return h;
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
        std::cerr << "No ISMRMRD header found in input stream." << std::endl;
        return 1;
    }

    w.EndData();

    return 0;
}
