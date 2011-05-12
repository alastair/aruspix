/*
 *  musneume_test.cpp
 *  aruspix
 *
 *  Created by Alastair Porter on 11-03-29.
 *  Copyright 2011 com.aruspix.www. All rights reserved.
 *
 */


#include <gtest/gtest.h>
#include <mei/mei.h>

#include "musneume.h"

TEST(NeumeTest, TestMakePunctum) {
	MeiElement ne = MeiElement("nemume");
    MeiElement no = MeiElement("note");
    ne.addAttribute(MeiAttribute("name", "punctum"));
    no.addAttribute(MeiAttribute("pname", "c"));
    ne.addChild(no);
    
    MusNeume mus = MusNeume(ne);
	ASSERT_EQ(mus.getType(), NEUME_TYPE_PUNCTUM);
    
    ne.addAttribute(MeiAttribute("variant", "inclinatum"));
    mus = MusNeume(ne);
	ASSERT_EQ(mus.getType(), NEUME_TYPE_PUNCTUM_INCLINATUM);
    
}

TEST(NeumeTest, TestMakeOtherTypes) {
	MeiElement ne = MeiElement("nemume");
    ne.addAttribute(MeiAttribute("name", "virga"));
    MusNeume mus = MusNeume(ne);
	ASSERT_EQ(mus.getType(), NEUME_TYPE_VIRGA);
    
    ne.removeAttribute("name");
    ne.addAttribute(MeiAttribute("name", "podatus"));
    mus = MusNeume(ne);
	ASSERT_EQ(mus.getType(), NEUME_TYPE_PODATUS);
    
    ne.removeAttribute("name");
    ne.addAttribute(MeiAttribute("name", "clivis"));
    mus = MusNeume(ne);
	ASSERT_EQ(mus.getType(), NEUME_TYPE_CLIVIS);
    
    ne.removeAttribute("name");
    ne.addAttribute(MeiAttribute("name", "porrectus"));
    mus = MusNeume(ne);
	ASSERT_EQ(mus.getType(), NEUME_TYPE_PORRECTUS);
    
    ne.removeAttribute("name");
    ne.addAttribute(MeiAttribute("name", "scandicus"));
    mus = MusNeume(ne);
	ASSERT_EQ(mus.getType(), NEUME_TYPE_SCANDICUS);
    
    ne.removeAttribute("name");
    ne.addAttribute(MeiAttribute("name", "torculus"));
    mus = MusNeume(ne);
	ASSERT_EQ(mus.getType(), NEUME_TYPE_TORCULUS);
}

TEST(NeumeTest, TestMakeBadNeume) {
    MeiElement ne = MeiElement("nemume");
    ne.addAttribute(MeiAttribute("name", "not-a-type"));
    
    try {
        MusNeume mus = MusNeume(ne);
        ASSERT_TRUE(false); // Shouldn't get here
    } catch (...) {
        ASSERT_TRUE(true);
    }
}

TEST(NeumeTest, TestMakeOneNoteElement) {
	MeiElement ne = MeiElement("nemume");
    MeiElement no1 = MeiElement("note");
    ne.addAttribute(MeiAttribute("name", "punctum"));
    no1.addAttribute(MeiAttribute("pname", "g"));
    no1.addAttribute(MeiAttribute("oct", "4"));
    ne.addChild(no1);
    
    MusNeume mus = MusNeume(ne);
    ASSERT_EQ(mus.pitch, 7);
    ASSERT_EQ(mus.oct, 4);
    ASSERT_EQ(mus.getPitches().size(), 1);
    ASSERT_EQ(mus.getPitches().at(0).getPitchDifference(), 0);
}

TEST(NeumeTest, TestMakeManyNoteElements) {
	MeiElement ne = MeiElement("nemume");
    MeiElement no1 = MeiElement("note");
    MeiElement no2 = MeiElement("note");
    MeiElement no3 = MeiElement("note");
    ne.addAttribute(MeiAttribute("name", "torculus"));
    no1.addAttribute(MeiAttribute("pname", "c"));
    no1.addAttribute(MeiAttribute("oct", "4"));
    no2.addAttribute(MeiAttribute("pname", "d"));
    no2.addAttribute(MeiAttribute("oct", "4"));
    no3.addAttribute(MeiAttribute("pname", "c"));
    no3.addAttribute(MeiAttribute("oct", "4"));
    ne.addChild(no1);
    ne.addChild(no2);
    ne.addChild(no3);
    
    MusNeume mus = MusNeume(ne);
	ASSERT_EQ(mus.getType(), NEUME_TYPE_TORCULUS);
    
    ASSERT_EQ(mus.getPitches().size(), 3);
    ASSERT_EQ(mus.pitch, 7);
    ASSERT_EQ(mus.oct, 4);
    ASSERT_EQ(mus.getPitches().at(0).getPitchDifference(), 0);
    ASSERT_EQ(mus.getPitches().at(1).getPitchDifference(), 1);
    ASSERT_EQ(mus.getPitches().at(2).getPitchDifference(), 0);
}

TEST(NeumeTest, TestSkipOctaveUp) {
	MeiElement ne = MeiElement("nemume");
    MeiElement no1 = MeiElement("note");
    MeiElement no2 = MeiElement("note");
    ne.addAttribute(MeiAttribute("name", "podatud"));
    no1.addAttribute(MeiAttribute("pname", "g"));
    no1.addAttribute(MeiAttribute("oct", "4"));
    no2.addAttribute(MeiAttribute("pname", "a"));
    no2.addAttribute(MeiAttribute("oct", "5"));
    ne.addChild(no1);
    ne.addChild(no2);
    
    MusNeume mus = MusNeume(ne);
	ASSERT_EQ(mus.getType(), NEUME_TYPE_PODATUS);
    
    ASSERT_EQ(mus.getPitches().size(), 2);
    ASSERT_EQ(mus.pitch, 7);
    ASSERT_EQ(mus.oct, 4);
    ASSERT_EQ(mus.getPitches().at(0).getPitchDifference(), 0);
    ASSERT_EQ(mus.getPitches().at(1).getPitchDifference(), 1);
}

TEST(NeumeElementTest, TestMakeElement) {
    MeiElement note = MeiElement("note");
    note.addAttribute(MeiAttribute("pname", "c"));
    note.addAttribute(MeiAttribute("oct", "4"));
    
    MusNeumeElement mus = MusNeumeElement(note, 1, 4);
    ASSERT_EQ(mus.getPitchDifference(), 0);
}

TEST(NeumeElementTest, TestMissingPitch) {
    MeiElement note = MeiElement("note");
    note.addAttribute(MeiAttribute("oct", "4"));
    try { 
        MusNeumeElement mus = MusNeumeElement(note, 1, 4);
        ASSERT_TRUE(false);
    } catch (...) {
        ASSERT_TRUE(true);
    }
}

TEST(NeumeElementTest, TestMissingOctave) {
    MeiElement note = MeiElement("note");
    note.addAttribute(MeiAttribute("pname", "c"));
    try {
        MusNeumeElement mus = MusNeumeElement(note, 1, 4);
        ASSERT_TRUE(false);
    } catch (...) {
        ASSERT_TRUE(true);
    }
}
